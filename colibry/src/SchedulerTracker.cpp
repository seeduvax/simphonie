/*
 * @file SchedulerTracker.cpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/colibry/SchedulerTracker.hpp"
#include <sstream>
#include "Smp/ISimulator.h"
#include "Smp/Services/IEventManager.h"
#include "Smp/Services/IScheduler.h"
#include "simdeck/Exception.hpp"

#define PRE_EP "preEventHandler"
#define POST_EP "postEventHandler"

namespace simphonie {
namespace colibry {

SchedulerTracker::SchedulerTracker(Smp::String8 name, Smp::String8 descr, Smp::IObject *parent) :
    Service(name, descr, parent),
    _start(0) {
    addEP(PRE_EP, "Handler for the Scheduler_PreEventExecute event.", this,
        &SchedulerTracker::epPreExec);
    addEP(POST_EP, "Handler for the Scheduler_PostEventExecute event.", this,
        &SchedulerTracker::epPostExec);
    addEP("logStats", "Log scheduling statistics.", this, &SchedulerTracker::epLogStats);
}

void SchedulerTracker::connect() {
    _scheduler = getSimulator()->GetScheduler();
    _timeKeeper = getSimulator()->GetTimeKeeper();
    auto evntMngr = getSimulator()->GetEventManager();
    evntMngr->Subscribe(evntMngr->QueryEventId("Scheduler_PreEventExecute"), GetEntryPoint(PRE_EP));
    evntMngr->Subscribe(evntMngr->QueryEventId("Scheduler_PostEventExecute"), GetEntryPoint(POST_EP));
}

void SchedulerTracker::epPreExec() {
    _start = _timeKeeper->GetZuluTime();
}

void SchedulerTracker::epPostExec() {
    auto stop = _timeKeeper->GetZuluTime();
    auto evId=_scheduler->GetCurrentEventId();
    auto it = _stats.find(evId);
    auto duration = stop - _start;
    if (it!=_stats.end()) {
        auto& stat=it->second;
        stat.duration = duration;
        stat.count++;
        if (stat.min > stat.duration) {
            stat.min = stat.duration;
        }
        if (stat.max < stat.duration) {
            stat.max = stat.duration;
        }
        stat.mean=(stat.mean * (stat.count - 1) + stat.duration) / stat.count; 
    }
    else {
        _stats.insert({evId, { duration, duration, duration, (Smp::Float64)duration, 1}});
        
    }
    _start = 0;
}

void SchedulerTracker::epLogStats() {
    std::ostringstream oss;
    oss << std::endl << "Event <id>\t(<#calls> calls):\t<min>\t <avg>\t <max>"
        << std::endl;
    for (auto event : _stats) {
        oss << "Event " << event.first
            << " \t(" << event.second.count << " calls):\t"
            << event.second.min << "\t " << event.second.mean << "\t " << event.second.max
            << std::endl;

    }
    logInfo(oss.str().c_str());
}

}  /* namespace colibry */
}  /* namespace simph */
