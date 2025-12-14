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
    Service(name, descr, parent), _currentStat(nullptr)
{
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
    if ( _currentStat != nullptr ) {
        throw simdeck::Exception(this,
            "Scheduler_PreEventExecute has been emitted while waiting for a Scheduler_PostEventExecute event.");
    }
    const auto start = _timeKeeper->GetZuluTime();
    auto evId=_scheduler->GetCurrentEventId();
    auto it = _stats.find(evId);
    if (it == _stats.end()) {
        _stats.insert({evId, { evId , {}}});
        _currentStat = &(_stats.find(evId)->second);
    }
    else {
        _currentStat = &it->second;
    }
    _currentStat->dates.push_back({start, 0});
}

void SchedulerTracker::epPostExec() {
    if ( _currentStat == nullptr ) {
        throw simdeck::Exception(this,
            "Scheduler_PostEventExecute has been emitted while waiting for a Scheduler_PreEventExecute event.");
    }
    _currentStat->dates.back().stop = _timeKeeper->GetZuluTime();
    _currentStat = nullptr;
}

void SchedulerTracker::epLogStats() {
    std::ostringstream oss;
    oss << "Event <id> (<#calls> calls): <min> - <avg> - <max>";
    for (auto event : _stats) {
        Smp::Duration min = INT64_MAX, max = 0, sum = 0;
        for (auto call : event.second.dates) {
            const auto duration = call.stop - call.start;
            sum += duration;
            if (duration < min) {
                min = duration;
            }
            if (duration > max) {
                max = duration;
            }
        }
        oss << std::endl << "Event " << event.second.id
            << " (" << event.second.dates.size() << " calls) : "
            << min << " - " << sum / event.second.dates.size() << " - " << max;
    }
    logInfo(oss.str().c_str());
}

}  /* namespace colibry */
}  /* namespace simph */
