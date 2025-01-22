/*
 * @file EnumerationType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/EnumerationType.hpp"
#include <string.h>
#include "simph/smpdk/ExDuplicateLiteral.hpp"
#include "simph/smpdk/ExDuplicateName.hpp"
#include "simph/smpdk/ExInvalidObjectName.hpp"

namespace simph {
namespace smpdk {
using namespace simph::smpdk;
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
}  // namespace smpdk
}  // namespace simph
