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
#include "simph/kern/ExInvalidPrimitiveType.hpp"
#include <iostream>

namespace Smp {
    std::ostream& operator << (std::ostream& os, const PrimitiveTypeKind& obj) {
        os << (Smp::Int32)obj <<"("
            << simph::kern::TypeRegistry::getPrimitiveTypeName(obj) << ")";
        return os;
    }
}

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
Smp::String8 TypeRegistry::getPrimitiveTypeName(Smp::PrimitiveTypeKind _type) {
    switch (_type) {
        case Smp::PrimitiveTypeKind::PTK_None:
            return "PTK_None";
        case Smp::PrimitiveTypeKind::PTK_Char8:
            return "PTK_Char8";
        case Smp::PrimitiveTypeKind::PTK_Bool:
            return "PTK_Bool";
        case Smp::PrimitiveTypeKind::PTK_Int8:
            return "PTK_Int8";
        case Smp::PrimitiveTypeKind::PTK_Int16:
            return "PTK_Int16";
        case Smp::PrimitiveTypeKind::PTK_Int32:
            return "PTK_Int32";
        case Smp::PrimitiveTypeKind::PTK_Int64:
            return "PTK_Int64";
        case Smp::PrimitiveTypeKind::PTK_UInt8:
            return "PTK_UInt8";
        case Smp::PrimitiveTypeKind::PTK_UInt16:
            return "PTK_UInt16";
        case Smp::PrimitiveTypeKind::PTK_UInt32:
            return "PTK_UInt32";
        case Smp::PrimitiveTypeKind::PTK_UInt64:
            return "PTK_UInt64";
        case Smp::PrimitiveTypeKind::PTK_Float32:
            return "PTK_Float32";
        case Smp::PrimitiveTypeKind::PTK_Float64:
            return "PTK_Float64";
        case Smp::PrimitiveTypeKind::PTK_Duration:
            return "PTK_Duration";
        case Smp::PrimitiveTypeKind::PTK_DateTime:
            return "PTK_DateTime";
        case Smp::PrimitiveTypeKind::PTK_String8:
            return "PTK_String8";
        default:
            return "InvalidType";
    }
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
        throw ExInvalidPrimitiveType(this,type);
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
