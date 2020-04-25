/*
 * @file EnumerationType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/EnumerationType.hpp"
#include "simph/smpdk/ExInvalidObjectName.hpp"
#include "simph/smpdk/ExDuplicateName.hpp"
#include "simph/kern/ExDuplicateLiteral.hpp"
#include <string.h>

namespace simph {
	namespace kern {
using namespace simph::smpdk;
// --------------------------------------------------------------------
// ..........................................................
EnumerationType::EnumerationType(Smp::Uuid uuid, Smp::PrimitiveTypeKind kind,
        Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
        Type(uuid,kind,sizeof(Smp::Int32),name,descr,parent) {
}
// ..........................................................
EnumerationType::~EnumerationType() {
}
// --------------------------------------------------------------------
// ..........................................................
void EnumerationType::AddLiteral(Smp::String8 name, Smp::String8 description,
                                Smp::Int32 value) {
    if (!Object::checkName(name)) {
        throw ExInvalidObjectName(this,name);
    }
    for (auto l: _literals) {
        if (strcmp(name,l.name)==0) {
            throw ExDuplicateName(this,name);
        }
        if (value==l.value) {
            throw ExDuplicateLiteral(this,l.name,l.value);
        }
    }
    struct literal l;
    l.name=name;
    l.description=description;
    l.value=value;
    _literals.push_back(l);
}
}} // namespace simph::kern
