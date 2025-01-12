/*
 * @file Scheduler.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */

#include "simph/kern/Scheduler.hpp"
#include <atomic>
#include "Smp/ISimulator.h"
#include "Smp/IOutputField.h"
#include "abs/profiler.h"
#include "assert.h"
#include "simph/kern/Resolver.hpp"
#include "simph/sys/Logger.hpp"

#define EV_NAME_PRE_EVENT_EXECUTE "Scheduler_PreEventExecute"
#define EV_NAME_POST_EVENT_EXECUTE "Scheduler_PostEventExecute"
#define DURATION_MAX INT64_MAX

namespace simph {
namespace kern {

// --------------------------------------------------------------------
// ..........................................................
// TODO consider a Schedule being a Smp::IObject and extend IScheduler interface
// to provide a schedule list view list, or put the schedule events into a 
// IContainer.
// 
class Scheduler::Schedule {
public:
    Schedule(const Smp::IEntryPoint* ep, Smp::Duration simTime, Scheduler* owner,
             const std::vector<Smp::IOutputField*>& fields, Smp::Duration period = 0, Smp::Int64 repeat = 0)
        : _ep(ep),
          _simTime(simTime),
          _owner(owner),
          _fields(fields),
          _period(period),
          _repeat(repeat),
          _completed(false) {
        static std::atomic<Smp::Services::EventId> _nextId(0);
        _id = _nextId++;
    }

    void setTime(Smp::Duration simTime) {
        _simTime = simTime;
    }
    inline Smp::Duration getTime() const {
        return _simTime;
    }
    inline void setPeriod(Smp::Duration period) {
        _period = period;
    }
    inline void setRepeat(Smp::Int64 repeat) {
        _repeat = repeat;
    }
    inline Smp::Services::EventId getId() const {
        return _id;
    }
    inline bool isCompleted() const {
        return _completed;
    }
    void run() {
        PROFILER_REGION("Schedule::run");
        {
            std::string epName = _ep->GetParent() != nullptr ? _ep->GetParent()->GetName() : "";
            epName = epName + ".";
            epName = epName + _ep->GetName();
            PROFILER_REGION(epName.c_str());
            _ep->Execute();
        }
        {
            PROFILER_REGION("Propagate data");
            for (auto f : _fields) {
                f->Push();
            }
            if (_repeat != 0) {
                if (_repeat > 0) {
                    _repeat -= 1;
                }
                if (_period > 0) {
                    setTime(getTime() + _period);
                    _owner->schedule(this);
                }
            }
            else {
                _completed = true;
            }
        }
    }

private:
    const Smp::IEntryPoint* _ep;
    Smp::Duration _simTime;
    Scheduler* _owner;
    std::vector<Smp::IOutputField*> _fields;
    Smp::Duration _period;
    Smp::Int64 _repeat;
    Smp::Services::EventId _id;
    bool _completed;
};
// --------------------------------------------------------------------
// ..........................................................
Scheduler::Scheduler(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Component(name, descr, parent),
      _eventMgr(nullptr),
      _run(false),
      _mutex(),
      _th(),
      _currentSchedule(nullptr),
      _scheduled(compareSchedule) {
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
bool Scheduler::compareSchedule(const Schedule* a, const Schedule* b) {
    // To define order:
    // - 1st check time (obviously)
    // - then if equals, check insert order (id is used since id is created and
    // incremented on 1st insertion.
    Smp::Duration ta = a->getTime();
    Smp::Duration tb = b->getTime();
    return ta < tb || (ta == tb && a->getId() < b->getId());
}
// ..........................................................
void Scheduler::schedule(Schedule* s) {
    {
        Synchronized(_mutex) _scheduled.insert(s);
        if (s->getTime() == 0) {
            // 0 schedule time is a marker of schedule through AddImmediateEvent,
            // restore the "now" schedule time to ensure next AddImmediateEvent
            // will also be insert front in schedule queue, and finally leave the
            // schedule in a consistent state before its execution.
            s->setTime(_timeKeeper->GetSimulationTime());
        }
    }
    _monitor.notify_all();
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
    // Use 0 as schedule time to "force" insert in front of the schedule queue.
    return schedule(entryPoint, 0, 0, 0);
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
    auto mySchedule = new Schedule(entryPoint, absoluteSimTime, this, flowFields, cycleTime, repeat);
    schedule(mySchedule);
    return mySchedule->getId();
}
// ..........................................................
Scheduler::Schedule* Scheduler::findSchedule(Smp::Services::EventId event, bool remove) {
    Schedule* res = nullptr;
    Synchronized(_mutex);
    if (_currentSchedule != nullptr && _currentSchedule->getId() == event) {
        res = _currentSchedule;
    }
    else {
        for (auto it = _scheduled.begin(); res == nullptr && it != _scheduled.end(); ++it) {
            if ((*it)->getId() == event) {
                res = *it;
                if (remove) {
                    _scheduled.erase(it);
                }
            }
        }
    }
    return res;
}
// ..........................................................
void Scheduler::schedule(Smp::Services::EventId event, Smp::Duration absoluteSimTime) {
    // TODO think about it is valid to do that from the
    // scheduled entry point itself. Or what does it means if an entry
    // point change its own simulation time (and only simulation time if
    // scheduled once (with no repeat).
    auto s = findSchedule(event, true);
    if (s) {
        s->setTime(absoluteSimTime);
        schedule(s);
    }
}
// ..........................................................
Smp::Services::EventId Scheduler::AddSimulationTimeEvent(const Smp::IEntryPoint* entryPoint,
                                                         Smp::Duration simulationTime, Smp::Duration cycleTime,
                                                         Smp::Int64 repeat) {
    return schedule(entryPoint, _timeKeeper->GetSimulationTime() + simulationTime, cycleTime, repeat);
};
// ..........................................................
Smp::Services::EventId Scheduler::AddMissionTimeEvent(const Smp::IEntryPoint* entryPoint, Smp::Duration missionTime,
                                                      Smp::Duration cycleTime, Smp::Int64 repeat) {
    return schedule(entryPoint, _timeKeeper->GetSimulationTime() + missionTime - _timeKeeper->GetMissionTime(),
                    cycleTime, repeat);
}
// ..........................................................
Smp::Services::EventId Scheduler::AddEpochTimeEvent(const Smp::IEntryPoint* entryPoint, Smp::DateTime epochTime,
                                                    Smp::Duration cycleTime, Smp::Int64 repeat) {
    return schedule(entryPoint, _timeKeeper->GetSimulationTime() + epochTime - _timeKeeper->GetEpochTime(), cycleTime,
                    repeat);
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
    schedule(event, simulationTime + _timeKeeper->GetSimulationTime());
}
// ..........................................................
void Scheduler::SetEventMissionTime(Smp::Services::EventId event, Smp::Duration missionTime) {
    schedule(event, _timeKeeper->GetSimulationTime() + missionTime - _timeKeeper->GetMissionTime());
}
// ..........................................................
void Scheduler::SetEventEpochTime(Smp::Services::EventId event, Smp::DateTime epochTime) {
    schedule(event, _timeKeeper->GetSimulationTime() + epochTime - _timeKeeper->GetEpochTime());
}
// ..........................................................
void Scheduler::SetEventZuluTime(Smp::Services::EventId event, Smp::DateTime zuluTime) {
    LOGE(
        "Scheduler::SetEventZuluTime not implemented!!! (unsure of what shall be really done and what are the use "
        "case.")
}
// ..........................................................
void Scheduler::SetEventCycleTime(Smp::Services::EventId event, Smp::Duration cycleTime) {
    Synchronized(_mutex);
    auto s = findSchedule(event);
    if (s) {
        s->setPeriod(cycleTime);
    }
}
// ..........................................................
void Scheduler::SetEventRepeat(Smp::Services::EventId event, Smp::Int64 repeat) {
    Synchronized(_mutex);
    auto s = findSchedule(event);
    if (s) {
        s->setRepeat(repeat);
    }
}
// ..........................................................
void Scheduler::RemoveEvent(Smp::Services::EventId event) {
    Synchronized(_mutex);
    Schedule* s = findSchedule(event, true);
    if (s != nullptr && s != _currentSchedule) {
        delete s;
    }
}
// ..........................................................
Smp::Services::EventId Scheduler::GetCurrentEventId() const {
    Synchronized(_mutex);
    if (_currentSchedule) {
        return _currentSchedule->getId();
    }
    return -1;
}
// ..........................................................
inline Smp::Duration Scheduler::getNextScheduledEventTime() const {
    if (!_scheduled.empty()) {
        return (*_scheduled.begin())->getTime();
    }
    return DURATION_MAX;
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
        if ((*it)->getId()==eventId) {
            return true;
        }
    }
    return false;
}
// ..........................................................
void Scheduler::step() {
    Schedule* toRun = nullptr;
    {
        Synchronized(_mutex);
        while (_run && getNextScheduledEventTime() >= DURATION_MAX) {
            MonitorWait(_monitor);
        }
        if (!_run) {
            // wait state exited because stop was requested
            return;
        }
    }
    _eventMgr->Emit(_preEventExecuteId);
    {
        Synchronized(_mutex);
        // after event emit, timekeeper should have updated current time,
        // run next event only if its scheduled time is not ahead the new
        // current simulation time.
        if (_run && getNextScheduledEventTime() <= _timeKeeper->GetSimulationTime()) {
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
        if (toRun->isCompleted()) {
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
        _th.reset(new simph::sys::Thread(GetName(), this));
        _th->start();
    }
}
// ..........................................................
void Scheduler::epLeaveExecuting() {
    {
        Synchronized(_mutex)
        if (_run==false) {
            return;
        }
        _run=false;
    }
    _monitor.notify_all();
    if (_th != nullptr) {
        if (!_th->isCurrentThread()) {
            _th->join();
            _th.reset();
        }
    }
}
}  // namespace kern
}  // namespace simph
