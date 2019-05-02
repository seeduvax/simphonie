/*
 * @file TypeRegistry.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/TypeRegistry.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
TypeRegistry::TypeRegistry(Smp::String8 name, Smp::String8 description,
                                    Smp::IObject* parent): 
                    Component(name,description,parent),
                    _types("types","",this) {
}
// ..........................................................
TypeRegistry::~TypeRegistry() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::Publication::IType* TypeRegistry::GetType(Smp::PrimitiveTypeKind type) const {
    for (auto t: _types) {
        if (t->GetPrimitiveTypeKind()==type) {
            return t;
        }
    }
    return nullptr;
}
// ..........................................................
Smp::Publication::IType* TypeRegistry::GetType(Smp::Uuid typeUuid) const {
    for (auto t: _types) {
        if (t->GetUuid()==typeUuid) {
            return t;
        }
    }
    return nullptr;
}

}} // namespace simph::kern
