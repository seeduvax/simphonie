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

namespace simphonie {
namespace colibry {

SimControl::SimControl(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Parent(name, descr, parent) {
}

SimControl::~SimControl() {
}


void SimControl::onChange(Smp::Float64 value) {
    if (std::abs(value) > std::numeric_limits<Smp::Float64>::epsilon()) {
        logInfo("Request simulation stop");
        getSimulator()->Hold(true);
    }
}

} /* namespace colibry */
} /* namespace simphonie */
