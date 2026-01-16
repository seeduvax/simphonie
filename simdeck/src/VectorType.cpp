/*
 * @file VectorType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/VectorType.hpp"
#include "simdeck/SimpleArrayField.hpp"
#include "Smp/IComponent.h"

namespace simdeck {
const Smp::Uuid VectorType::UuidVector(0, 0, 0, { 'v','e','c','t','o','r'});

// --------------------------------------------------------------------
// ..........................................................
VectorType::VectorType(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent,
                     Smp::UInt64 allocationSize, Smp::Publication::IType* type)
    : Parent(VectorType::UuidVector, name, descr, parent, allocationSize, type) {}
// ..........................................................
VectorType::~VectorType() {}
// --------------------------------------------------------------------
// ..........................................................
}  // namespace simdeck
