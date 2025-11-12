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

Schedule::Schedule(Scheduler* scheduler, const Smp::IEntryPoint* ep, const std::vector<Smp::IOutputField*>& fields,
                   Smp::Duration absoluteSimTime, Smp::Duration period, Smp::Int64 repeat)
    : _scheduler(scheduler),
      _ep(ep),
      _fields(fields),
      _absoluteSimTime(absoluteSimTime),
      _period(period),
      _repeat(repeat),
      _completed(false),
      _counterActivation(0),
      _isWaiting(false) {
    static std::atomic<Smp::Services::EventId> _nextId(0);
    _id = _nextId++;
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

}  /* namespace kern */
}  /* namespace simph */
