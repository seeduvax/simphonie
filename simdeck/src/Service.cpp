/*
 * @file Service.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/Service.hpp"

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
Service::Service(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
            Parent(name, descr, parent) {
}
// ..........................................................
Service::~Service() {
}

} // namespace simdeck
