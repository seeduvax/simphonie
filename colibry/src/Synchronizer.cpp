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
#include <chrono>
#include <thread>
#include "Smp/ISimulator.h"
#include "Smp/Services/IEventManager.h"
#include "Smp/Services/IScheduler.h"

#include "simphonie/kern/Scheduler.hpp" /* TODO use #include "simdeck/smpext/?.hpp" instead */

#define INIT_EP_NAME "init"
#define STEP_EP_NAME "step"

namespace simphonie {
namespace colibry {

Synchronizer::Synchronizer(Smp::String8 name, Smp::String8 description, Smp::IObject* parent)
    : simdeck::Component(name, description, parent), _periodSmp(0), _overflowCount(0), _margin(0) {
    addEP(INIT_EP_NAME, "Internal use only.", this, &Synchronizer::_init);
    addEP(STEP_EP_NAME, "Internal use only.", this, &Synchronizer::step);
}

void Synchronizer::connect() {
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterExecutingId,
                                                 GetEntryPoint(INIT_EP_NAME));
    const auto eventId =
        getSimulator()->GetScheduler()->AddSimulationTimeEvent(GetEntryPoint(STEP_EP_NAME), 0L, _periodSmp, -1L);
    dynamic_cast<simphonie::kern::Scheduler*>(getSimulator()->GetScheduler())->SetEventPriority(eventId, 0);
    _period = _Duration(_periodSmp);
}

void Synchronizer::publish(Smp::IPublication* receiver) {
    receiver->PublishField("margin", "Difference between the expected and current zulu time values", &_margin,
                           Smp::ViewKind::VK_All, false, false, true);
    receiver->PublishField("overflowCounter", "Counter of cycling overflow.", &_overflowCount, Smp::ViewKind::VK_All,
                           false, false, true);
    receiver->PublishField("period", "Cycling period in nanoseconds.", &_periodSmp, Smp::ViewKind::VK_All, false, true,
                           false);
}

void Synchronizer::_init() {
    _goal = _Clock::now() + _period;
}

void Synchronizer::step() {
    const auto goal = _goal;
    _goal += _period;
    {
        const auto time = std::chrono::duration_cast<_Duration>(_Clock::now().time_since_epoch());
        _margin = (goal.time_since_epoch() - time).count();
        if (_margin < 0) {
            _overflowCount++;
        }
    }
    {
        std::mutex mutex;
        std::unique_lock<std::mutex> lock(mutex);
        std::condition_variable cv;
        cv.wait_until(lock, goal);
    }
}

} /* namespace colibry */
} /* namespace simphonie */
