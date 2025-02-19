/*
 * @file ExInvalidType.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/ExInvalidType.hpp"

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidType::ExInvalidType(const Smp::IObject* sender, Smp::String8 msg) {
    setName("InvalidType");
    setSender(sender);
    setDescription(msg);
    setMessage();
}
// ..........................................................
ExInvalidType::~ExInvalidType() {
}

} // namespace simdeck
