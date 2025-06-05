/*
 * @file Synchronizer.cpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/colibry/Synchronizer.hpp"
#include "Smp/ISimulator.h"
#include "Smp/Services/IEventManager.h"
#include "Smp/Services/IScheduler.h"

#include "simphonie/kern/Scheduler.hpp" /* TODO use #include "simdeck/smpext/?.hpp" instead */

#define INIT_EP_NAME "init"
#define STEP_EP_NAME "step"

#define NSEC_PER_SEC 1000000000L

namespace simphonie {
namespace colibry {

Synchronizer::Synchronizer(Smp::String8 name, Smp::String8 description, Smp::IObject* parent)
    : simdeck::Component(name, description, parent), _periodSmp(0), _overflowCount(0), _margin(0), _startSec(true) {
    addEP(INIT_EP_NAME, "Internal use only.", this, &Synchronizer::_init);
    addEP(STEP_EP_NAME, "Internal use only.", this, &Synchronizer::step);
}

void Synchronizer::connect() {
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterExecutingId,
                                                 GetEntryPoint(INIT_EP_NAME));
    const auto eventId =
        getSimulator()->GetScheduler()->AddSimulationTimeEvent(GetEntryPoint(STEP_EP_NAME), 0L, _periodSmp, -1L);
    dynamic_cast<simphonie::kern::Scheduler*>(getSimulator()->GetScheduler())->SetEventPriority(eventId, 0);
    _period = {_periodSmp / NSEC_PER_SEC, _periodSmp % NSEC_PER_SEC};
}

void Synchronizer::publish(Smp::IPublication* receiver) {
    receiver->PublishField("margin", "Difference between the expected and current zulu time values", &_margin,
                           Smp::ViewKind::VK_All, true, false, true);
    receiver->PublishField("overflowCounter", "Counter of cycling overflow.", &_overflowCount, Smp::ViewKind::VK_All,
                           true, false, true);
    receiver->PublishField("period", "Cycling period in nanoseconds.", &_periodSmp, Smp::ViewKind::VK_All, true, true,
                           false);
    receiver->PublishField("startSec", "true to start on the next second.", &_startSec, Smp::ViewKind::VK_All, true,
                           true, false);
}

void Synchronizer::_init() {
    clock_gettime(CLOCK_MONOTONIC, &_goal);
    if (_startSec) {
        struct timespec rt;
        clock_gettime(CLOCK_REALTIME, &rt);
        _goal.tv_nsec += NSEC_PER_SEC - rt.tv_nsec;
        _goal.tv_sec += _goal.tv_nsec / NSEC_PER_SEC;
        _goal.tv_nsec %= NSEC_PER_SEC;
    }
    _updateGoal();
}

void Synchronizer::_updateGoal() {
    _goal.tv_nsec += _period.tv_nsec;
    _goal.tv_sec += _period.tv_sec + _goal.tv_nsec / NSEC_PER_SEC;
    _goal.tv_nsec %= NSEC_PER_SEC;
}

void Synchronizer::step() {
    {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        _margin = now.tv_sec * NSEC_PER_SEC + now.tv_nsec - (_goal.tv_sec * NSEC_PER_SEC + _goal.tv_nsec);
    }
    if (_margin < 0) {
        _overflowCount++;
    }
    {
        const struct timespec goal = _goal;
        _updateGoal();
        while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &goal, NULL)) {}
    }
}

} /* namespace colibry */
} /* namespace simphonie */
