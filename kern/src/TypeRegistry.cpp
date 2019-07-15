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
#include "simph/kern/EnumerationType.hpp"
#include "simph/kern/ClassType.hpp"
#include "simph/kern/StructureType.hpp"
#include "simph/kern/ArrayType.hpp"
#include "simph/kern/ExTypeAlreadyRegistered.hpp"
#include "simph/kern/ExInvalidPrimitiveType.hpp"
#include "simph/sys/Logger.hpp"
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
    if (type==Smp::PrimitiveTypeKind::PTK_Float32) {
        res=new Type(typeUuid,type,sizeof(Smp::Float32),name,descr,this);
    }
    else {
        res=new Type(typeUuid,type,sizeof(Smp::Float64),name,descr,this);
    }
    // TODO create Type subclass to store min, max, unit, etc. Once I can
    // find what to do with such additional attributes....
    _types.push_back(res);
    return res;
}
// ..........................................................
Smp::Publication::IType* TypeRegistry::AddIntegerType(
                Smp::String8 name, Smp::String8 descr,
                Smp::Uuid typeUuid, Smp::Int64 minimum, Smp::Int64 maximum,
                Smp::String8 unit, Smp::PrimitiveTypeKind type) {
    Smp::Publication::IType* res=GetType(typeUuid);
    if (res!=nullptr) {
        throw ExTypeAlreadyRegistered(this,name,res);
    }
    int size=0;
    switch (type) {
            case Smp::PrimitiveTypeKind::PTK_Int8:
                size=sizeof(Smp::Int8);
                break;
            case Smp::PrimitiveTypeKind::PTK_Int16:
                size=sizeof(Smp::Int16);
                break;
            case Smp::PrimitiveTypeKind::PTK_Int32:
                size=sizeof(Smp::Int32);
                break;
            case Smp::PrimitiveTypeKind::PTK_Int64:
                size=sizeof(Smp::Int64);
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt8:
                size=sizeof(Smp::Int8);
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt16:
                size=sizeof(Smp::Int16);
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt32:
                size=sizeof(Smp::Int32);
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt64:
                size=sizeof(Smp::Int64);
                break;
            default:
                throw ExInvalidPrimitiveType(this,type);
    }
    res=new Type(typeUuid,type,size,name,descr,this);
    // TODO create Type subclass to store min, max, unit, etc. Once I can
    // find what to do with such additional attributes....
    _types.push_back(res);
    return res;
}
// ..........................................................
Smp::Publication::IEnumerationType* TypeRegistry::AddEnumerationType(
                Smp::String8 name, Smp::String8 descr,
                Smp::Uuid typeUuid, Smp::Int16 memorySize) {
    Smp::Publication::IType* ex=GetType(typeUuid);
    if (ex!=nullptr) {
        throw ExTypeAlreadyRegistered(this,name,ex);
    }
    Smp::PrimitiveTypeKind type=Smp::PrimitiveTypeKind::PTK_None;
    switch (memorySize) {
        case 1: type=Smp::PrimitiveTypeKind::PTK_Int8;
           break;
        case 2: type=Smp::PrimitiveTypeKind::PTK_Int16;
           break;
        case 4: type=Smp::PrimitiveTypeKind::PTK_Int32;
           break;
        case 8: type=Smp::PrimitiveTypeKind::PTK_Int64;
           // There is an issue with SMP interface definition, this size
           // is not really useable since IEnumerationType interface AddLiteral
           // value use Smp::Int32 type to define the value.
           break;
    }
    Smp::Publication::IEnumerationType* res=nullptr;
    if (type!=Smp::PrimitiveTypeKind::PTK_None) {
        res=new EnumerationType(typeUuid,type,name,descr,this);
        _types.push_back(res);
    }
    else {
        LOGE("Invalid memory size for type "<<name);
    }
    return res;
}
// ..........................................................
Smp::Publication::IArrayType* TypeRegistry::AddArrayType(
            Smp::String8 name, Smp::String8 description, Smp::Uuid typeUuid,
            Smp::Uuid itemTypeUuid, Smp::Int64 itemSize,
            Smp::Int64 arrayCount) {
    Smp::Publication::IType* ex=GetType(typeUuid);
    if (ex!=nullptr) {
        throw ExTypeAlreadyRegistered(this,name,ex);
    }
    Smp::Publication::IType* itemType=GetType(itemTypeUuid);
    if (itemType!=nullptr) {
        Smp::Publication::IArrayType* res=
                new ArrayType(typeUuid,name,description,this,
                            itemSize,arrayCount,itemType);
        _types.push_back(res);
        return res;
    }
    return nullptr;
}
// ..........................................................
Smp::Publication::IType* TypeRegistry::AddStringType(
        Smp::String8 name, Smp::String8 description, Smp::Uuid typeUuid,
        Smp::Int64 length) {
    Smp::Publication::IType* res=GetType(typeUuid);
    if (res!=nullptr) {
        throw ExTypeAlreadyRegistered(this,name,res);
    }
    res=new Type(typeUuid,Smp::PrimitiveTypeKind::PTK_String8,
                                       length,name,description,this);
    _types.push_back(res);
    return res;
}
// ..........................................................
Smp::Publication::IStructureType* TypeRegistry::AddStructureType(
        Smp::String8 name, Smp::String8 description, Smp::Uuid typeUuid) {
    Smp::Publication::IType* ex=GetType(typeUuid);
    if (ex!=nullptr) {
        throw ExTypeAlreadyRegistered(this,name,ex);
    }
    Smp::Publication::IStructureType* res=
                        new StructureType(typeUuid,name,description,this);
    _types.push_back(res);
    return res;
}
// ..........................................................
Smp::Publication::IClassType* TypeRegistry::AddClassType(
        Smp::String8 name, Smp::String8 description, Smp::Uuid typeUuid,
        Smp::Uuid baseClassUuid) {
    Smp::Publication::IType* ex=GetType(typeUuid);
    if (ex!=nullptr) {
        throw ExTypeAlreadyRegistered(this,name,ex);
    }
    // TODO take care of base class type
    Smp::Publication::IClassType* res=
                        new ClassType(typeUuid,name,description,this);
    _types.push_back(res);
    return res;
}

}} // namespace simph::kern
