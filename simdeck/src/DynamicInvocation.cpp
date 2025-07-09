/*
 * @file DynamicInvocation.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/DynamicInvocation.hpp"

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
DynamicInvocation::DynamicInvocation(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent ):
        Parent(name, descr, parent) {
}
// ..........................................................
DynamicInvocation::~DynamicInvocation() {
}
// --------------------------------------------------------------------
// ..........................................................
void DynamicInvocation::Invoke(Smp::IRequest* request) {
}

} // namespace simdeck
