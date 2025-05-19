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
#include "simphonie/kern/Simulator.hpp"
#include "simphonie/kern/EventManager.hpp"
#include "simphonie/kern/Scheduler.hpp"
#include "simdeck/Exception.hpp"
#include <sstream>

#define PRE_EP "preEventHandler"
#define POST_EP "postEventHandler"

namespace simphonie {
namespace colibry {

SchedulerTracker::SchedulerTracker(Smp::String8 name, Smp::String8 descr, Smp::IObject *parent) :
    Service(name, descr, parent), _currentEvent(nullptr)
{
    _scheduler = dynamic_cast<simdeck::smpext::IObservableScheduler*>
        (getSimulator()->GetScheduler());
    if (!_scheduler) {
        throw simdeck::Exception(this,
            "simphonie::colibry::ScheduleTracker cannot be use with a non simdeck::smpext::IObservableScheduler scheduler.");
    }
    _timeKeeper = getSimulator()->GetTimeKeeper();
    addEP(PRE_EP, "Handler for the Scheduler_PreEventExecute event.", this,
        &SchedulerTracker::_preEventEP);
    addEP(POST_EP, "Handler for the Scheduler_PostEventExecute event.", this,
        &SchedulerTracker::_postEventEP);
    addEP("logStats", "Log scheduling statistics.", this, &SchedulerTracker::_logStats);
}

void SchedulerTracker::connect() {
    auto evntMngr = getSimulator()->GetEventManager();
    evntMngr->Subscribe(evntMngr->QueryEventId("Scheduler_PreEventExecute"), GetEntryPoint(PRE_EP));
    evntMngr->Subscribe(evntMngr->QueryEventId("Scheduler_PostEventExecute"), GetEntryPoint(POST_EP));
}

void SchedulerTracker::_preEventEP() {
    if (_currentEvent) {
        throw simdeck::Exception(this,
            "Scheduler_PreEventExecute has been emitted while waiting for a Scheduler_PostEventExecute event.");
    }
    const auto start = _timeKeeper->GetZuluTime();
    auto schedule = _scheduler->GetSchedule();
    auto it = _events.find(schedule->GetId());
    if (it == _events.end()) {
        _events.insert({schedule->GetId(), {schedule->GetId(), schedule->GetName(), {}}});
        it = _events.find(schedule->GetId());
    }
    _currentEvent = &it->second;
    _currentEvent->calls.push_back({start, 0});
}

void SchedulerTracker::_postEventEP() {
    if (!_currentEvent) {
        throw simdeck::Exception(this,
            "Scheduler_PostEventExecute has been emitted while waiting for a Scheduler_PreEventExecute event.");
    }
    _currentEvent->calls.back().stop = _timeKeeper->GetZuluTime();
    _currentEvent = nullptr;
}

void SchedulerTracker::_logStats() {
    std::ostringstream oss;
    oss << "Event <id> (<name>) (<#calls> calls): <min> - <avg> - <max>";
    for (auto event : _events) {
        Smp::Duration min = INT64_MAX, max = 0, sum = 0;
        for (auto call : event.second.calls) {
            const auto duration = call.stop - call.start;
            sum += duration;
            if (duration < min) {
                min = duration;
            }
            if (duration > max) {
                max = duration;
            }
        }
        oss << std::endl << "Event " << event.second.id << " (" << event.second.name
            << ") (" << event.second.calls.size() << " calls) : "
            << min << " - " << sum / event.second.calls.size() << " - " << max;
    }
    logInfo(oss.str().c_str());
}

}  /* namespace colibry */
}  /* namespace simph */
