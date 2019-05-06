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
#include "simph/kern/Type.hpp"
#include "simph/kern/ExTypeAlreadyRegistered.hpp"

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
// --------------------------------------------------------------------
// ..........................................................
Smp::Publication::IType* TypeRegistry::AddFloatType(
                Smp::String8 name, Smp::String8 descr,
                Smp::Uuid typeUuid, Smp::Float64 minimum, Smp::Float64 maximum,
                Smp::Bool minInclusive, Smp::Bool maxInclusive,
                Smp::String8 unit, Smp::PrimitiveTypeKind type) {
    if (type!=Smp::PrimitiveTypeKind::PTK_Float32 &&
            type!=Smp::PrimitiveTypeKind::PTK_Float64) {
        // TODO throw Smp::Publication::InvalidPrimitiveType
    }
    Smp::Publication::IType* res=GetType(typeUuid);
    if (res!=nullptr) {
        throw ExTypeAlreadyRegistered(this,name,res);
    }
    res=new Type(typeUuid,type,name,descr,this);
    // TODO create Type subclass to store min, max, unit, etc. Once I can
    // find what to do with such additional attributes....
    _types.push_back(res);
    return res;
}

}} // namespace simph::kern
