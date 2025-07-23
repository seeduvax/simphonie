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
#include <thread>

#define INIT_EP_NAME "init"
#define STEP_EP_NAME "step"

#define NSEC_PER_SEC 1000000000L

namespace simphonie {
namespace colibry {

// --------------------------------------------------------------------
// ..........................................................
Synchronizer::Synchronizer(Smp::String8 name, Smp::String8 description, Smp::IObject* parent)
    : simdeck::Component(name, description, parent), _periodSmp(0), _overflowCount(0), _margin(0), _startSec(true) {
    addEP(INIT_EP_NAME, "Internal use only.", this, &Synchronizer::_init);
    addEP(STEP_EP_NAME, "Internal use only.", this, &Synchronizer::step);
}

// ..........................................................
void Synchronizer::connect() {
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterExecutingId,
                                                 GetEntryPoint(INIT_EP_NAME));
    const auto eventId =
        getSimulator()->GetScheduler()->AddSimulationTimeEvent(GetEntryPoint(STEP_EP_NAME), 0L, _periodSmp, -1L);
}

// ..........................................................
void Synchronizer::publish(Smp::IPublication* receiver) {
    receiver->PublishField("margin", "Difference between the expected and current zulu time values", &_margin,
                           Smp::ViewKind::VK_All, false, false, true);
    receiver->PublishField("overflowCounter", "Counter of cycling overflow.", &_overflowCount, Smp::ViewKind::VK_All,
                           false, false, true);
    receiver->PublishField("period", "Cycling period in nanoseconds.", &_periodSmp, Smp::ViewKind::VK_All, false, true,
                           false);
    receiver->PublishField("startSec", "true to start on the next second.", &_startSec, Smp::ViewKind::VK_All, false,
                           true, false);
}

// ..........................................................
void Synchronizer::_init() {
    auto now = std::chrono::system_clock::now();
    if (_startSec) {
        auto sec=std::chrono::time_point_cast<std::chrono::seconds>(now);
        sec+=std::chrono::seconds(1);
        _goal=std::chrono::system_clock::time_point(sec) + std::chrono::nanoseconds(_periodSmp);
        std::this_thread::sleep_until(sec);
    }
    else {
        _goal = now + std::chrono::nanoseconds(_periodSmp);
    }
}

// ..........................................................
void Synchronizer::step() {
    {
        auto timeLeft=_goal - std::chrono::system_clock::now();
        _margin=std::chrono::duration_cast<std::chrono::nanoseconds>(timeLeft).count();
        // TODO set overflow threshold configurable
        if (_margin > 1000) {
            std::this_thread::sleep_until(_goal);
        }
        else {
            _overflowCount++;
        }
        _goal = _goal + std::chrono::nanoseconds(_periodSmp);
    }
}

} /* namespace colibry */
} /* namespace simphonie */
