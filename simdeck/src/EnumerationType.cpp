/*
 * @file EnumerationType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/EnumerationType.hpp"
#include <string.h>
#include "simdeck/ExDuplicateLiteral.hpp"
#include "simdeck/ExDuplicateName.hpp"
#include "simdeck/ExInvalidObjectName.hpp"

namespace simdeck {
using namespace simdeck;
// --------------------------------------------------------------------
// ..........................................................
EnumerationType::EnumerationType(Smp::Uuid uuid, Smp::PrimitiveTypeKind kind, Smp::String8 name, Smp::String8 descr,
                                 Smp::IObject* parent)
    : Type(uuid, kind, sizeof(Smp::Int32), name, descr, parent) {}
// ..........................................................
EnumerationType::~EnumerationType() {}
// --------------------------------------------------------------------
// ..........................................................
void EnumerationType::AddLiteral(Smp::String8 name, Smp::String8 description, Smp::Int32 value) {
    Object::checkName(name);
    for (auto l : _literals) {
        if (strcmp(name, l.name) == 0) {
            throw ExDuplicateName(this, name);
        }
        if (value == l.value) {
            throw ExDuplicateLiteral(this, l.name, l.value);
        }
    }
    struct literal l;
    l.name = name;
    l.description = description;
    l.value = value;
    _literals.push_back(l);
}
}  // namespace simdeck
