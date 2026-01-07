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
    : simdeck::Component(name, description, parent), _periodSmp(10000000), _overflowCount(0), _margin(0), _marginThresh(100), _startSec(true) {
    addEP(INIT_EP_NAME, "Internal use only.", this, &Synchronizer::epInit);
    addEP(STEP_EP_NAME, "Internal use only.", this, &Synchronizer::epStep);
}

// ..........................................................
void Synchronizer::connect() {
    // TODO unsure this is strategy to init on enter executing always OK 
    // regarding various SMP implementation. It depends if the scheduler threads
    // starts concurrently from the event manager notification loop or after
    // end of the loop.
    // May need to add immediate event on levaing stand by. To be checked with
    // the standard about how the scheduler start should be sequenced.
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterExecutingId,
                                                 GetEntryPoint(INIT_EP_NAME));

    // TODO consider another scheduling policy. This one can't ensure the sync
    // entry point is scheduled first or last for its next simulation time
    // activation, leading to unexpected synchronization in the middle of the
    // list of entry point scheduled at the same simulation time. 
    // Activating synchro on simulation time change could be a better option.
    const auto eventId =
        getSimulator()->GetScheduler()->AddSimulationTimeEvent(GetEntryPoint(STEP_EP_NAME), 0L, _periodSmp, -1L);
}

// ..........................................................
void Synchronizer::publish(Smp::IPublication* receiver) {
    receiver->PublishField("Margin", "Difference between the expected and current zulu time values", &_margin,
                           Smp::ViewKind::VK_All, false, false, true);
    receiver->PublishField("OverflowCount", "Counter of cycling overflow.", &_overflowCount, Smp::ViewKind::VK_All,
                           false, false, true);
    receiver->PublishField("Period", "Cycling period in nanoseconds.", &_periodSmp, Smp::ViewKind::VK_All, false, true,
                           false);
    receiver->PublishField("StartSec", "true to start on the next second.", &_startSec, Smp::ViewKind::VK_All, false,
                           true, false);
    receiver->PublishField("MarginThreshold", "The thread sleep if the margin to wait for is above this threshold.",
                           &_marginThresh, Smp::ViewKind::VK_All, false, true, false);
}

// ..........................................................
void Synchronizer::epInit() {
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
void Synchronizer::epStep() {
    {
        auto timeLeft=_goal - std::chrono::system_clock::now();
        _margin=std::chrono::duration_cast<std::chrono::nanoseconds>(timeLeft).count();
        if (_margin > _marginThresh) {
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
