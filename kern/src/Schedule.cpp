/*
 * @file Schedule.cpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/Schedule.hpp"
#include "abs/profiler.h"

#define START_EP "StartOnEvent"
#define STOP_EP "StopOnEvent"

namespace simphonie {
namespace kern {

Schedule::Schedule(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent, const Smp::IEntryPoint* ep,
                   const std::vector<Smp::IOutputField*>& fields, Smp::Duration absoluteSimTime, Smp::Duration period,
                   Smp::Int64 repeat, Smp::UInt64 priority)
    : Object(name, descr, parent),
      _scheduler(dynamic_cast<Scheduler*>(parent)),
      _ep(ep),
      _fields(fields),
      _absoluteSimTime(absoluteSimTime),
      _period(period),
      _repeat(repeat),
      _priority(priority),
      _completed(false),
      _counterActivation(0),
      _startEventId(NO_EVENTID),
      _stopEventId(NO_EVENTID),
      _isWaiting(false) {
    if (_scheduler == nullptr) {
        throw simdeck::ExInvalidParent(this, GetParent(), nullptr);
    }
    static std::atomic<Smp::Services::EventId> _nextId(0);
    _id = _nextId++;
    addEP(START_EP, "EntryPoint for starting on event.", this, &Schedule::_epStartOnEvent);
    addEP(STOP_EP, "EntryPoint for stoping on event.", this, &Schedule::_epStopOnEvent);
}

void Schedule::setTime(Smp::Duration absoluteSimTime, Smp::Bool updateScheduler) {
    _absoluteSimTime = absoluteSimTime;
    if (updateScheduler) {
        _scheduler->updateSchedule(_id);
    }
}

void Schedule::setIsWaiting(Smp::Bool isWaiting, Smp::Bool updateScheduler) {
    _isWaiting = isWaiting;
    if (updateScheduler) {
        _scheduler->updateSchedule(_id);
    }
}

void Schedule::setPriority(Smp::UInt64 priority, Smp::Bool updateScheduler) {
    _priority = priority;
    if (updateScheduler) {
        _scheduler->updateSchedule(_id);
    }
}

void Schedule::setStartEventId(Smp::Services::EventId startEventId) {
    _startEventId = startEventId;
    if (_startEventId != NO_EVENTID) {
        setIsWaiting(true, true);
        _scheduler->getSimulator()->GetEventManager()->Subscribe(_startEventId, GetEntryPoint(START_EP));
    } else {
        setIsWaiting(false, true);
        _scheduler->getSimulator()->GetEventManager()->Unsubscribe(_startEventId, GetEntryPoint(START_EP));
    }
}

void Schedule::setStopEventId(Smp::Services::EventId stopEventId) {
    _stopEventId = stopEventId;
    if (_stopEventId != NO_EVENTID) {
        _scheduler->getSimulator()->GetEventManager()->Subscribe(_stopEventId, GetEntryPoint(STOP_EP));
    } else {
        _scheduler->getSimulator()->GetEventManager()->Unsubscribe(_stopEventId, GetEntryPoint(STOP_EP));
    }
}

void Schedule::run() {
    PROFILER_REGION("Schedule::run");
    {
        std::string epName = _ep->GetParent() != nullptr ?
            _ep->GetParent()->GetName() : "";
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
        if (_counterActivation != _repeat) {
            if (_period > 0) {
                setTime(_absoluteSimTime + _period);
            }
        }
        else {
            _completed = true;
        }
    }
    _counterActivation++;
}

void Schedule::_epStartOnEvent() {
    _absoluteSimTime = _scheduler->getSimulator()->GetTimeKeeper()->GetSimulationTime();
    setIsWaiting(false);
}

}  /* namespace kern */
}  /* namespace simph */
