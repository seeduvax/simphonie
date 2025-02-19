/*
 * @file ClassType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/ClassType.hpp"

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
ClassType::ClassType(Smp::Uuid uuid, Smp::Publication::ITypeRegistry* typeReg, Smp::String8 name, Smp::String8 description,
                     Smp::IObject* parent)
    : StructureType(uuid, typeReg, name, description, parent) {}
// ..........................................................
ClassType::~ClassType() {}

}  // namespace simdeck
