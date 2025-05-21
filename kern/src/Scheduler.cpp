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
#include "simphonie/kern/Schedule.hpp"
#include "simphonie/sys/Logger.hpp"

#define EV_NAME_PRE_EVENT_EXECUTE "Scheduler_PreEventExecute"
#define EV_NAME_POST_EVENT_EXECUTE "Scheduler_PostEventExecute"
#define DURATION_MAX INT64_MAX
#define CONTAINER_NAME "Observers"

namespace simphonie {
namespace kern {

// --------------------------------------------------------------------
// ..........................................................
Scheduler::Scheduler(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Component(name, descr, parent),
      _eventMgr(nullptr),
      _run(false),
      _mutex(),
      _th(),
      _currentSchedule(nullptr),
      _scheduled(),
      _activableCount(0) {
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
        _scheduled.insert(s);
        if (!s->isWaiting()) {
            _activableCount++;
        }
    }
    for (auto observer : _observers) {
        observer->notifyScheduled(s);
    }
    _monitor.notify_all();
}

void Scheduler::publish(Smp::IPublication* receiver) {
    receiver->PublishField("activableCount", "Number of activable event in the scheduled queue.", &_activableCount,
                           Smp::ViewKind::VK_All, false, false, true);
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
    return schedule(entryPoint, _timeKeeper->GetSimulationTime(), 0, 0, -1);
}
// ..........................................................
Smp::Services::EventId Scheduler::schedule(const Smp::IEntryPoint* entryPoint, Smp::Duration absoluteSimTime,
                                           Smp::Duration cycleTime, Smp::Int64 repeat, Smp::UInt64 priority) {
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
    auto mySchedule = new Schedule(entryPoint->GetName(), "", this, entryPoint, flowFields, absoluteSimTime, cycleTime,
                                   repeat, priority);
    schedule(mySchedule);
    return mySchedule->GetId();
}
// ..........................................................
Schedule* Scheduler::findSchedule(Smp::Services::EventId event, bool remove) {
    Synchronized(_mutex);
    if (_currentSchedule != nullptr && _currentSchedule->GetId() == event) {
        return _currentSchedule;
    }
    else {
        for (auto it = _scheduled.begin(); it != _scheduled.end(); ++it) {
            if ((*it)->GetId() == event) {
                auto res = *it;
                if (remove) {
                    _scheduled.erase(it);
                    if (!res->isWaiting()) {
                        _activableCount--;
                    }
                }
                return res;
            }
        }
    }
    return nullptr;
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

void Scheduler::updateSchedule(Smp::Services::EventId eventId) {
    auto s = findSchedule(eventId, true);
    if (s != nullptr) {
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
    schedule(event, _timeKeeper->GetSimulationTime() + simulationTime);
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

void Scheduler::SetEventStartOnEvent(Smp::Services::EventId eventId, Smp::Services::EventId triggerEventId) {
    auto s = findSchedule(eventId);
    if (s) {
        s->setStartEventId(triggerEventId);
    }
}

void Scheduler::SetEventStopOnEvent(Smp::Services::EventId eventId, Smp::Services::EventId triggerEventId) {
    auto s = findSchedule(eventId);
    if (s) {
        s->setStopEventId(triggerEventId);
    }
}

void Scheduler::SetEventPriority(Smp::Services::EventId eventId, Smp::UInt64 priority) {
    auto s = findSchedule(eventId);
    if (s) {
        s->setPriority(priority);
    }
}

// ..........................................................
void Scheduler::RemoveEvent(Smp::Services::EventId event) {
    Schedule* s = findSchedule(event, true);
    if (s != nullptr && s != _currentSchedule) {
        for (auto observer : _observers) {
            observer->notifyCanceled(s->GetId());
        }
        delete s;
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
    if (_activableCount > 0) {
        return (*_scheduled.begin())->GetTime();
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
        if ((*it)->GetId() == eventId) {
            return true;
        }
    }
    return false;
}

void Scheduler::RegisterObserver(smpext::ISchedulerObserver* observer) {
    _observers.push_back(observer);
}

void Scheduler::RemoveObserver(smpext::ISchedulerObserver* observer) {
    for (auto it = _observers.begin(); it != _observers.end(); ++it) {
        if (*it = observer) {
            _observers.erase(it);
        }
    }
}

const smpext::ISchedule* Scheduler::GetSchedule() const {
    if (_currentSchedule != nullptr) {
        return _currentSchedule;
    }
    if (_scheduled.size() > 0) {
        return *_scheduled.begin();
    }
    return nullptr;
}

// ..........................................................
void Scheduler::step() {
    Schedule* toRun = nullptr;
    {
        Synchronized(_mutex);
        while (_run && getNextScheduledEventTime() >= DURATION_MAX) {
            logInfo("No activable event left. The scheduler has been paused.");
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
            _activableCount--;
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
            for (auto observer : _observers) {
                observer->notifyCompleted(eventId);
            }
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
