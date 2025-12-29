/*
 * @file ExInvalidCycleTime.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/ExInvalidCycleTime.hpp"

namespace simphonie {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidCycleTime::ExInvalidCycleTime(Smp::IObject* sender) {
    setSender(sender);
    setName("InvalidCycleTime");
}
// ..........................................................
ExInvalidCycleTime::~ExInvalidCycleTime() {
}

}} // namespace simphonie::kern
