/*
 * @file TypeRegistry.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/TypeRegistry.hpp"

#include <iostream>

#include "simdeck/ArrayType.hpp"
#include "simdeck/ClassType.hpp"
#include "simdeck/EnumerationType.hpp"
#include "simdeck/ExInvalidPrimitiveType.hpp"
#include "simdeck/StructureType.hpp"
#include "simdeck/Type.hpp"
#include "simphonie/kern/ExTypeAlreadyRegistered.hpp"

// TODO to be reconsidered, smp logger should be used
#include "simphonie/sys/Logger.hpp"

// TODO good news, since there is no AddType but only Add<supertype>Type, the
// registered IType instance are only simdeck instances, then it should be safe
// to extend IType to include a field factory in it.

namespace simphonie {
namespace kern {

using namespace simdeck;

// --------------------------------------------------------------------
// ..........................................................
TypeRegistry::TypeRegistry(Smp::String8 name, Smp::String8 description, Smp::IObject* parent)
    : Component(name, description, parent), _types("types", "", this) {
    // Register primitive types

    _types.push_back(new Type(Smp::Uuids::Uuid_Char8, Smp::PrimitiveTypeKind::PTK_Char8, sizeof(Smp::Char8), "Char8",
                              "One byte char data type"));
    _types.push_back(
        new Type(Smp::Uuids::Uuid_Bool, Smp::PrimitiveTypeKind::PTK_Bool, sizeof(Smp::Bool), "Bool", "Bool data type"));
    _types.push_back(new Type(Smp::Uuids::Uuid_Int8, Smp::PrimitiveTypeKind::PTK_Int8, sizeof(Smp::Int8), "Int8",
                              "One byte signed int data type"));
    _types.push_back(new Type(Smp::Uuids::Uuid_Int16, Smp::PrimitiveTypeKind::PTK_Int16, sizeof(Smp::Int16), "Int16",
                              "Two bytes signed int data type"));
    _types.push_back(new Type(Smp::Uuids::Uuid_Int32, Smp::PrimitiveTypeKind::PTK_Int32, sizeof(Smp::Int32), "Int32",
                              "Four bytes signed int data type"));
    _types.push_back(new Type(Smp::Uuids::Uuid_Int64, Smp::PrimitiveTypeKind::PTK_Int64, sizeof(Smp::Int64), "Int64",
                              "Eight bytes signed int data type"));
    _types.push_back(new Type(Smp::Uuids::Uuid_UInt8, Smp::PrimitiveTypeKind::PTK_UInt8, sizeof(Smp::UInt8), "UInt8",
                              "One byte unsigned int data type"));
    _types.push_back(new Type(Smp::Uuids::Uuid_UInt16, Smp::PrimitiveTypeKind::PTK_UInt16, sizeof(Smp::UInt16),
                              "UInt16", "Two bytes unsigned int data type"));
    _types.push_back(new Type(Smp::Uuids::Uuid_UInt32, Smp::PrimitiveTypeKind::PTK_UInt32, sizeof(Smp::UInt32),
                              "UInt32", "Four bytes unsigned int data type"));
    _types.push_back(new Type(Smp::Uuids::Uuid_UInt64, Smp::PrimitiveTypeKind::PTK_UInt64, sizeof(Smp::UInt64),
                              "UInt64", "Eight bytes unsigned int data type"));
    _types.push_back(new Type(Smp::Uuids::Uuid_Float32, Smp::PrimitiveTypeKind::PTK_Float32, sizeof(Smp::Float32),
                              "Float32", "Four bytes float data type"));
    _types.push_back(new Type(Smp::Uuids::Uuid_Float64, Smp::PrimitiveTypeKind::PTK_Float64, sizeof(Smp::Float64),
                              "Float64", "Eight bytes float data type"));
    // TODO add missing type (at least string ?)
}
// ..........................................................
TypeRegistry::~TypeRegistry() {
    // nothing to deleted, created types are in _types and shall be deleted
    // thanks to the std::unique_ptr smart pointers.
}
// --------------------------------------------------------------------
// ..........................................................
size_t TypeRegistry::getPrimitiveTypeSize(Smp::PrimitiveTypeKind type) {
    switch (type) {
        case Smp::PrimitiveTypeKind::PTK_Char8:
            return sizeof(Smp::Char8);
        case Smp::PrimitiveTypeKind::PTK_Bool:
            return sizeof(Smp::Bool);
        case Smp::PrimitiveTypeKind::PTK_Int8:
            return sizeof(Smp::Int8);
        case Smp::PrimitiveTypeKind::PTK_Int16:
            return sizeof(Smp::Int16);
        case Smp::PrimitiveTypeKind::PTK_Int32:
            return sizeof(Smp::Int32);
        case Smp::PrimitiveTypeKind::PTK_Int64:
            return sizeof(Smp::Int64);
        case Smp::PrimitiveTypeKind::PTK_UInt8:
            return sizeof(Smp::UInt8);
        case Smp::PrimitiveTypeKind::PTK_UInt16:
            return sizeof(Smp::UInt16);
        case Smp::PrimitiveTypeKind::PTK_UInt32:
            return sizeof(Smp::UInt32);
        case Smp::PrimitiveTypeKind::PTK_UInt64:
            return sizeof(Smp::UInt64);
        case Smp::PrimitiveTypeKind::PTK_Float32:
            return sizeof(Smp::Float32);
        case Smp::PrimitiveTypeKind::PTK_Float64:
            return sizeof(Smp::Float64);
        case Smp::PrimitiveTypeKind::PTK_Duration:
            return sizeof(Smp::Duration);
        case Smp::PrimitiveTypeKind::PTK_DateTime:
            return sizeof(Smp::DateTime);
        default:
            return 0;
    }
}
// ..........................................................
Smp::String8 TypeRegistry::getPrimitiveTypeName(Smp::PrimitiveTypeKind type) {
    switch (type) {
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
    for (auto t : _types) {
        if (t->GetPrimitiveTypeKind() == type) {
            return t;
        }
    }
    return nullptr;
}
// ..........................................................
Smp::Publication::IType* TypeRegistry::GetType(Smp::Uuid typeUuid) const {
    for (auto t : _types) {
        if (t->GetUuid() == typeUuid) {
            return t;
        }
    }
    return nullptr;
}
// --------------------------------------------------------------------
// ..........................................................
Smp::Publication::IType* TypeRegistry::AddFloatType(Smp::String8 name, Smp::String8 descr, Smp::Uuid typeUuid,
                                                    Smp::Float64 minimum, Smp::Float64 maximum, Smp::Bool minInclusive,
                                                    Smp::Bool maxInclusive, Smp::String8 unit,
                                                    Smp::PrimitiveTypeKind type) {
    if (type != Smp::PrimitiveTypeKind::PTK_Float32 && type != Smp::PrimitiveTypeKind::PTK_Float64) {
        throw ExInvalidPrimitiveType(this, type);
    }
    Smp::Publication::IType* res = GetType(typeUuid);
    if (res != nullptr) {
        throw ExTypeAlreadyRegistered(this, name, res);
    }
    if (type == Smp::PrimitiveTypeKind::PTK_Float32) {
        res = new Type(typeUuid, type, sizeof(Smp::Float32), name, descr, this);
    }
    else {
        res = new Type(typeUuid, type, sizeof(Smp::Float64), name, descr, this);
    }
    // TODO create Type subclass to store min, max, unit, etc. Once I can
    // find what to do with such additional attributes....
    _types.push_back(res);
    return res;
}
// ..........................................................
Smp::Publication::IType* TypeRegistry::AddIntegerType(Smp::String8 name, Smp::String8 descr, Smp::Uuid typeUuid,
                                                      Smp::Int64 minimum, Smp::Int64 maximum, Smp::String8 unit,
                                                      Smp::PrimitiveTypeKind type) {
    Smp::Publication::IType* res = GetType(typeUuid);
    if (res != nullptr) {
        throw ExTypeAlreadyRegistered(this, name, res);
    }
    int size = 0;
    switch (type) {
        case Smp::PrimitiveTypeKind::PTK_Int8:
            size = sizeof(Smp::Int8);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int16:
            size = sizeof(Smp::Int16);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int32:
            size = sizeof(Smp::Int32);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int64:
            size = sizeof(Smp::Int64);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt8:
            size = sizeof(Smp::Int8);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt16:
            size = sizeof(Smp::Int16);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt32:
            size = sizeof(Smp::Int32);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt64:
            size = sizeof(Smp::Int64);
            break;
        default:
            throw ExInvalidPrimitiveType(this, type);
    }
    res = new Type(typeUuid, type, size, name, descr, this);
    // TODO create Type subclass to store min, max, unit, etc. Once I can
    // find what to do with such additional attributes....
    _types.push_back(res);
    return res;
}
// ..........................................................
Smp::Publication::IEnumerationType* TypeRegistry::AddEnumerationType(
                        Smp::String8 name,
                        Smp::String8 descr,
                        Smp::Uuid typeUuid) {
    Smp::Publication::IType* ex = GetType(typeUuid);
    if (ex != nullptr) {
        throw ExTypeAlreadyRegistered(this, name, ex);
    }
    Smp::Publication::IEnumerationType* res =
        // TODO 2nd arg should no more be needed.
        new EnumerationType(typeUuid, Smp::PrimitiveTypeKind::PTK_Int32, name, descr, this);
    _types.push_back(res);
    return res;
}
// ..........................................................
Smp::Publication::IArrayType* TypeRegistry::AddArrayType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid,
                Smp::Uuid itemTypeUuid,
                Smp::UInt64 itemSize,
                Smp::UInt64 arrayCount,
                Smp::Bool simpleArray) {
    // TODO complete behavior regarding simpleArray arg value.
    Smp::Publication::IType* ex = GetType(typeUuid);
    if (ex != nullptr) {
        throw ExTypeAlreadyRegistered(this, name, ex);
    }
    Smp::Publication::IType* itemType = GetType(itemTypeUuid);
    if (itemType != nullptr) {
        Smp::Publication::IArrayType* res =
            new ArrayType(typeUuid, name, description, this, itemSize, arrayCount, itemType);
        _types.push_back(res);
        return res;
    }
    return nullptr;
}
// ..........................................................
Smp::Publication::IStringType* TypeRegistry::AddStringType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid,
                Smp::UInt64 length) {
    Smp::Publication::IType* res = GetType(typeUuid);
    if (res != nullptr) {
        throw ExTypeAlreadyRegistered(this, name, res);
    }
    /*
     * TODO to be restored with proper String Type
        res = new Type(typeUuid, Smp::PrimitiveTypeKind::PTK_String8, length, name, description, this);
        _types.push_back(res);
        return res;
     */
    return nullptr;
}
// ..........................................................
Smp::Publication::IStructureType* TypeRegistry::AddStructureType(Smp::String8 name, Smp::String8 description,
                                                                 Smp::Uuid typeUuid) {
    Smp::Publication::IType* ex = GetType(typeUuid);
    if (ex != nullptr) {
        throw ExTypeAlreadyRegistered(this, name, ex);
    }
    Smp::Publication::IStructureType* res = new StructureType(typeUuid, this, name, description, this);
    _types.push_back(res);
    return res;
}
// ..........................................................
Smp::Publication::IClassType* TypeRegistry::AddClassType(Smp::String8 name, Smp::String8 description,
                                                         Smp::Uuid typeUuid, Smp::Uuid baseClassUuid) {
    Smp::Publication::IType* ex = GetType(typeUuid);
    if (ex != nullptr) {
        throw ExTypeAlreadyRegistered(this, name, ex);
    }
    // TODO take care of base class type
    Smp::Publication::IClassType* res = new ClassType(typeUuid, this, name, description, this);
    _types.push_back(res);
    return res;
}

}  // namespace kern
}  // namespace simph
