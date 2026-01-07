/*
 * @file SimControl.cpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/colibry/SimControl.hpp"

#include <limits>

#include "Smp/ISimulator.h"
#include "Smp/Services/IEventManager.h"

namespace simphonie {
namespace colibry {

// --------------------------------------------------------------------
// ..........................................................
SimControl::SimControl(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Parent(name, descr, parent) {
}

// ..........................................................
SimControl::~SimControl() {
}


// --------------------------------------------------------------------
// ..........................................................
void SimControl::connect() {
    Parent::connect();
    getSimulator()->GetEventManager()->Subscribe(
            Smp::Services::IEventManager::SMP_PostSimTimeChangeId,
            GetEntryPoint(EVALUATE_EP_NAME)
        );
}
// ..........................................................
void SimControl::onEvaluate() {
    if (getChanged() && 
            std::abs(getOut()) > std::numeric_limits<Smp::Float64>::epsilon()) {
        logInfo("Request simulation stop");
        getSimulator()->Hold(true);
    }
}

} /* namespace colibry */
} /* namespace simphonie */
