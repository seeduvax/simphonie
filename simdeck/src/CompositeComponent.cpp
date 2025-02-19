/*
 * @file CompositeComponent.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/CompositeComponent.hpp"

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
CompositeComponent::CompositeComponent(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
            Parent(name, descr, parent) {
}
// ..........................................................
CompositeComponent::~CompositeComponent() {
}

} // namespace simdeck
