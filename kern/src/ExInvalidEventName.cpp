/*
 * @file ExInvalidEventName.cpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/ExInvalidEventName.hpp"

namespace simphonie {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidEventName::ExInvalidEventName(Smp::IObject* sender) {
    setSender(sender);
    setName("InvalidEventName");
}
// ..........................................................
ExInvalidEventName::~ExInvalidEventName() {
}

}} // namespace simphonie::kern
