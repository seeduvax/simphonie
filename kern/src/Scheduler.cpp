/*
 * @file Scheduler.cpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */

#include "simphonie/kern/Scheduler.hpp"
#include <atomic>
#include <vector>
#include "Smp/IOutputField.h"
#include "Smp/ISimulator.h"
#include "assert.h"
#include "simphonie/kern/ExInvalidEventId.hpp"
#include "simphonie/kern/Resolver.hpp"
#include "simphonie/sys/Logger.hpp"
#include "abs/profiler.h"

#define EV_NAME_PRE_EVENT_EXECUTE "Scheduler_PreEventExecute"
#define EV_NAME_POST_EVENT_EXECUTE "Scheduler_PostEventExecute"
#define CONTAINER_NAME "Observers"
#define IMMEDIATE_SIMULATION_TIME -2


// TODO : to be reconsidered according the comments from the IScheduler header.
//   - use of -1 as IMMEDIATE_SIMULATION_TIME magic value.
//   - simulation time saturation policy. It is specified a simulation time too
//     far shall throw an invalid simulation time exception.

namespace simphonie {
namespace kern {

Scheduler::Schedule::Schedule(Scheduler* scheduler, const Smp::IEntryPoint* ep, const std::vector<Smp::IOutputField*>& fields,
                   Smp::Duration absoluteSimTime, Smp::Duration period, Smp::Int64 repeat)
    : _scheduler(scheduler),
      _ep(ep),
      _fields(fields),
      _absoluteSimTime(absoluteSimTime),
      _period(period),
      _repeat(repeat),
      _completed(false) { 
    static std::atomic<Smp::Services::EventId> _nextId(0);
    _id = _nextId++;
}

void Scheduler::Schedule::setTime(Smp::Duration absoluteSimTime) {
    _absoluteSimTime = absoluteSimTime;
    _scheduler->schedule(this);
}

void Scheduler::Schedule::run() {
    PROFILER_REGION("Schedule::run");
    {
        std::string epName = _ep->GetParent() != nullptr ?
            _ep->GetParent()->GetName() : "";
        epName = epName + ".";
        epName = epName + _ep->GetName();
        PROFILER_REGION(epName.c_str());
        // update next activation date first,to let next activation time
        // overridable with Scheduler::SetEventSimulationTime during the entry
        // point execution itself.
        // see clause 5.3.3 l.2.f (p91 in ECSS-E-ST-40-07C Rev.1 5 august 2025)
        _completed = _repeat == 0;
        if ( _repeat > 0) {
            _repeat--;
        }
        if ( !_completed && _period > 0 ) {
            setTime(
                _period >= (DURATION_MAX - _absoluteSimTime) 
                ? DURATION_MAX
                : _absoluteSimTime + _period);
        }
        // now run entry point.
        _ep->Execute();
    }
    {
        // entry point executed. Push the related output fields.
        PROFILER_REGION("Propagate data");
        for (auto f : _fields) {
            f->Push();
        }
    }
}

// --------------------------------------------------------------------
// ..........................................................
Scheduler::Scheduler(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Component(name, descr, parent),
      _eventMgr(nullptr),
      _run(false),
      _mutex(),
      _th(),
      _currentSchedule(nullptr),
      _scheduled() {
    _epEnterExecuting=EntryPoint::Create("enterExecuting",
                                "simulation enter execute event entry point", 
                                this, &Scheduler::epEnterExecuting);
    _epLeaveExecuting=EntryPoint::Create("leaveExecuting",
                                "simulation leave execute event entry point",
                                this, &Scheduler::epLeaveExecuting);
}
// ..........................................................
Scheduler::~Scheduler() {
    // Ensure it is no more running
    epLeaveExecuting();
    // delete all remaining schedules.
    for (auto s : _scheduled) {
        delete s;
    }
    delete _epEnterExecuting;
    delete _epLeaveExecuting;
}
// --------------------------------------------------------------------
// ..........................................................
void Scheduler::schedule(Schedule* s) {
    {
        Synchronized(_mutex);
        bool erased=false;
        for (auto it=_scheduled.begin(); it!=_scheduled.end(); ++it) {
            if (s->GetId() == (*it)->GetId()) {
                _scheduled.erase(it);
                break;
            }
        }
        _scheduled.insert(s);
    }
    _monitor.notify_all();
}

void Scheduler::publish(Smp::IPublication* receiver) {
    receiver->PublishField("autoStop", "Scheduler stops the simulation when no more entry points are scheduled.", &_autoStop,
                           Smp::ViewKind::VK_All, false, true, false);
}

// ..........................................................
void Scheduler::connect() {
    _timeKeeper = getSimulator()->GetTimeKeeper();
    _eventMgr = getSimulator()->GetEventManager();
    _eventMgr->Subscribe(Smp::Services::IEventManager::SMP_EnterExecutingId,
            _epEnterExecuting);
    _eventMgr->Subscribe(Smp::Services::IEventManager::SMP_LeaveExecutingId,
            _epLeaveExecuting);
    _preEventExecuteId=_eventMgr->QueryEventId(EV_NAME_PRE_EVENT_EXECUTE);
    _postEventExecuteId=_eventMgr->QueryEventId(EV_NAME_POST_EVENT_EXECUTE);
}

// ..........................................................
Smp::Services::EventId Scheduler::AddImmediateEvent(const Smp::IEntryPoint* entryPoint) {
    return schedule(entryPoint, IMMEDIATE_SIMULATION_TIME, 0, 0);
}
// ..........................................................
Smp::Services::EventId Scheduler::schedule(const Smp::IEntryPoint* entryPoint, Smp::Duration absoluteSimTime,
                                           Smp::Duration cycleTime, Smp::Int64 repeat) {
    std::vector<Smp::IOutputField*> flowFields;
    auto resolver = dynamic_cast<Resolver*>(getSimulator()->GetResolver());
    auto obj = dynamic_cast<Smp::IComponent*>(entryPoint->GetParent());
    if (obj!=nullptr) {
        auto fields = obj->GetFields();
        if (fields != nullptr) {
            for (auto itField = fields->begin(); itField != fields->end(); ++itField) {
                Smp::IOutputField* dfField = dynamic_cast<Smp::IOutputField*>(*itField);
                if (dfField != nullptr && dfField->IsOutput()) {
                    flowFields.push_back(dfField);
                }
            }
        }
    }
    auto mySchedule = new Schedule(this, entryPoint, flowFields, absoluteSimTime, cycleTime, repeat);
    schedule(mySchedule);
    return mySchedule->GetId();
}
// ..........................................................
Scheduler::Schedule* Scheduler::findSchedule(Smp::Services::EventId event) {
    Synchronized(_mutex);
    if (_currentSchedule != nullptr && _currentSchedule->GetId() == event) {
        return _currentSchedule;
    }
    else {
        for (auto it = _scheduled.begin(); it != _scheduled.end(); ++it) {
            if ((*it)->GetId() == event) {
                return *it;
            }
        }
    }
    return nullptr;
}
// ..........................................................
void Scheduler::schedule(Smp::Services::EventId event, Smp::Duration absoluteSimTime) {
    auto s = findSchedule(event);
    if (s) {
        s->setTime(absoluteSimTime);
        schedule(s);
    }
}

// ..........................................................
Smp::Services::EventId Scheduler::AddSimulationTimeEvent(const Smp::IEntryPoint* entryPoint,
                                                         Smp::Duration simulationTime, Smp::Duration cycleTime,
                                                         Smp::Int64 repeat) {
    return schedule(entryPoint, getAbsoluteTime(simulationTime), cycleTime, repeat);
};
// ..........................................................
Smp::Services::EventId Scheduler::AddMissionTimeEvent(const Smp::IEntryPoint* entryPoint, Smp::Duration missionTime,
                                                      Smp::Duration cycleTime, Smp::Int64 repeat) {
    return schedule(entryPoint, getAbsoluteTime(missionTime - _timeKeeper->GetMissionTime()), cycleTime, repeat);
}
// ..........................................................
Smp::Services::EventId Scheduler::AddEpochTimeEvent(const Smp::IEntryPoint* entryPoint, Smp::DateTime epochTime,
                                                    Smp::Duration cycleTime, Smp::Int64 repeat) {
    return schedule(entryPoint, getAbsoluteTime(epochTime - _timeKeeper->GetEpochTime()), cycleTime, repeat);
}
// ..........................................................
Smp::Services::EventId Scheduler::AddZuluTimeEvent(const Smp::IEntryPoint* entryPoint, Smp::DateTime zuluTime,
                                                   Smp::Duration cycleTime, Smp::Int64 repeat) {
    LOGE(
        "Scheduler::AddZuluTimeEvent not implemented!!! (unsure of what shall be really done and what are the use "
        "cases.")
    return -1;
}
// ..........................................................
Smp::Services::EventId Scheduler::AddRelativeZuluTimeEvent(
                    const Smp::IEntryPoint* entryPoint,
                    Smp::DateTime zuluTimedelay,
                    Smp::Duration cycleTime,
                    Smp::Int64 repeat) {
    LOGE(
        "Scheduler::AddRelativeZuluTimeEvent not implemented!!! (unsure of what shall be really done and what are the use "
        "cases.")
    return -1;
}
// ..........................................................
void Scheduler::SetEventSimulationTime(Smp::Services::EventId event, Smp::Duration simulationTime) {
    schedule(event, getAbsoluteTime(simulationTime));
}
// ..........................................................
void Scheduler::SetEventMissionTime(Smp::Services::EventId event, Smp::Duration missionTime) {
    schedule(event, getAbsoluteTime(missionTime - _timeKeeper->GetMissionTime()));
}
// ..........................................................
void Scheduler::SetEventEpochTime(Smp::Services::EventId event, Smp::DateTime epochTime) {
    schedule(event, getAbsoluteTime(epochTime - _timeKeeper->GetEpochTime()));
}
// ..........................................................
void Scheduler::SetEventZuluTime(Smp::Services::EventId event, Smp::DateTime zuluTime) {
    LOGE(
        "Scheduler::SetEventZuluTime not implemented!!! (unsure of what shall be really done and what are the use "
        "case.")
}
// ..........................................................
void Scheduler::SetEventCycleTime(Smp::Services::EventId event, Smp::Duration cycleTime) {
    auto s = findSchedule(event);
    if (s) {
        s->setPeriod(cycleTime);
    }
}
// ..........................................................
void Scheduler::SetEventRepeat(Smp::Services::EventId event, Smp::Int64 repeat) {
    auto s = findSchedule(event);
    if (s) {
        s->setRepeat(repeat);
    }
}

// ..........................................................
void Scheduler::RemoveEvent(Smp::Services::EventId event) {
    Synchronized(_mutex);
    for (auto it=_scheduled.begin(); it!=_scheduled.end(); ++it) {
        if (event==(*it)->GetId()) {
            _scheduled.erase(it);
        }
    }
}
// ..........................................................
Smp::Services::EventId Scheduler::GetCurrentEventId() const {
    Synchronized(_mutex);
    if (_currentSchedule) {
        return _currentSchedule->GetId();
    }
    return -1;
}
// ..........................................................
inline Smp::Duration Scheduler::getNextScheduledEventTime() const {
    if (!_scheduled.empty()) {
        auto time=(*_scheduled.begin())->GetTime();
        return time!=IMMEDIATE_SIMULATION_TIME?time:
                // when next event is immediate event, the simulation
                // time is unchanged then next event simulation time is
                // the current time from the time keeper.
                _timeKeeper->GetSimulationTime();
    }
    return -1;
}
// ..........................................................
Smp::Duration Scheduler::GetNextScheduledEventTime() const {
    Synchronized(_mutex);
    return getNextScheduledEventTime();
}
// ..........................................................
Smp::Bool Scheduler::IsEventScheduled(Smp::Services::EventId eventId) const {
    Synchronized(_mutex);
    for (auto it = _scheduled.begin(); it != _scheduled.end(); ++it) {
        if ((*it)->GetId() == eventId) {
            return true;
        }
    }
    return false;
}

// ..........................................................
void Scheduler::step() {
    Schedule* toRun = nullptr;
    bool completed=false;
    {
        Synchronized(_mutex);
        if (!_autoStop) {
            while (_run && getNextScheduledEventTime() != -1) {
                logInfo("No activable event left. The scheduler has been paused.");
                MonitorWait(_monitor);
            }
        }
        if (!_run) {
            // wait state exited because stop was requested
            return;
        }
        completed=getNextScheduledEventTime() == -1 
                || getNextScheduledEventTime()==DURATION_MAX;
    }
    if (_autoStop && completed) {
        getSimulator()->Hold(true);
        return;
    }
    _eventMgr->Emit(_preEventExecuteId);
    {
        Synchronized(_mutex);
        // after event emit, timekeeper should have updated current time,
        // run next event only if its scheduled time is not ahead the new
        // current simulation time.
        if (_run && getNextScheduledEventTime() <= _timeKeeper->GetSimulationTime() 
                 && getNextScheduledEventTime()!=-1) {
            _currentSchedule = *_scheduled.begin();
            _scheduled.erase(_scheduled.begin());
            toRun = _currentSchedule;
        }
    }
    if (toRun != nullptr) {
        toRun->run();
        _eventMgr->Emit(_postEventExecuteId);
        Synchronized(_mutex);
        if (toRun != _currentSchedule) {
            logWarning(
                "Scheduler state changed while running an entry point. "
                " Next scheduling may be corrupted.");
        }
        _currentSchedule = nullptr;
        const auto eventId = toRun->GetId();
        if (toRun->IsCompleted()) {
            delete toRun;
        }
    }
}
// ..........................................................
void Scheduler::run() {
    _run = true;
    while (_run) {
        step();
    }
}
// --------------------------------------------------------------------
// ..........................................................
void Scheduler::epEnterExecuting() {
    Synchronized(_mutex);
    if (!_run) {
        _th.reset(new simphonie::sys::Thread(GetName(), this));
        _th->start();
    }
}
// ..........................................................
void Scheduler::epLeaveExecuting() {
    {
        Synchronized(_mutex)
        _run=false;
    }
    _monitor.notify_all();
    if (_th != nullptr && !_th->isCurrentThread()) {
        _th->join();
        _th.reset();
    }
}

bool Scheduler::_compareSchedule::operator()(const Schedule* a, const Schedule* b) const {
    return *a < *b;
}

}  // namespace kern
}  // namespace simph
