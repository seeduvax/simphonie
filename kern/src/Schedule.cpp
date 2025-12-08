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
      _counterActivation(0) {
    static std::atomic<Smp::Services::EventId> _nextId(0);
    _id = _nextId++;
}

void Schedule::setTime(Smp::Duration absoluteSimTime) {
    _absoluteSimTime = absoluteSimTime;
    _scheduler->schedule(this);
}

void Schedule::run() {
    PROFILER_REGION("Schedule::run");
    {
        std::string epName = _ep->GetParent() != nullptr ?
            _ep->GetParent()->GetName() : "";
        epName = epName + ".";
        epName = epName + _ep->GetName();
        PROFILER_REGION(epName.c_str());
        _counterActivation++;
        // update next activation date first,to let next activation time
        // overridable with Scheduler::SetEventSimulationTime during the entry
        // point execution itself.
        // see clause 5.3.3 l.2.f (p91 in ECSS-E-ST-40-07C Rev.1 5 august 2025)
        if (_counterActivation != _repeat) {
            if (_period > 0) {
                setTime(
                    _period >= (DURATION_MAX - _absoluteSimTime) 
                    ? DURATION_MAX
                    : _absoluteSimTime + _period);
            }
        }
        else {
            _completed = true;
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

}  /* namespace kern */
}  /* namespace simph */
