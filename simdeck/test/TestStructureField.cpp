/*
 * @file TestStructureField.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "abs/test.h"
#include "simdeck/StructureField.hpp"
#include "Smp/PrimitiveTypes.h"
#include "simdeck/SimpleType.hpp"

namespace test {
using namespace simdeck;
SimpleType _byteType(Smp::Uuids::Uuid_UInt8, Smp::PrimitiveTypeKind::PTK_UInt8, "Byte");
SimpleType _intType(Smp::Uuids::Uuid_Int64, Smp::PrimitiveTypeKind::PTK_Int64, "Int64");
SimpleType _numberType(Smp::Uuids::Uuid_Float64, Smp::PrimitiveTypeKind::PTK_Float64, "Number");

class TypeReg: public Smp::Publication::ITypeRegistry {
public:
    // Smp::Publication::ITypeRegistry
    Smp::Publication::IType* GetType(Smp::PrimitiveTypeKind type) const override {
        if (type == Smp::PrimitiveTypeKind::PTK_UInt8) return &_byteType;
        if (type == Smp::PrimitiveTypeKind::PTK_Int64) return &_intType;
        if (type == Smp::PrimitiveTypeKind::PTK_Float64) return &_numberType;
        return nullptr;
    }
    Smp::Publication::IType* GetType(Smp::Uuid typeUuid) const override {
        if (typeUuid == Smp::Uuids::Uuid_UInt8) return &_byteType;
        if (typeUuid == Smp::Uuids::Uuid_Int64) return &_intType;
        if (typeUuid == Smp::Uuids::Uuid_Float64) return &_numberType;
        return nullptr;
    }
    Smp::Publication::IType* AddFloatType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid,
                Smp::Float64 minimum,
                Smp::Float64 maximum,
                Smp::Bool minInclusive,
                Smp::Bool maxInclusive,
                Smp::String8 unit,
                Smp::PrimitiveTypeKind type) override {
        return nullptr;
    }
    Smp::Publication::IType* AddIntegerType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid,
                Smp::Int64 minimum,
                Smp::Int64 maximum,
                Smp::String8 unit,
                Smp::PrimitiveTypeKind type = Smp::PrimitiveTypeKind::PTK_Int32) override {
        return nullptr;
    }
    Smp::Publication::IEnumerationType* AddEnumerationType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid) override {
        return nullptr;
    }
    Smp::Publication::IArrayType* AddArrayType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid,
                Smp::Uuid itemTypeUuid,
                Smp::UInt64 itemSize,
                Smp::UInt64 arrayCount,
                Smp::Bool simpleArray = false) override {
        return nullptr;
    }
    Smp::Publication::IStringType* AddStringType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid,
                Smp::UInt64 length) override {
        return nullptr;
    }
    Smp::Publication::IStructureType* AddStructureType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid) override {
        return nullptr;
    }
    Smp::Publication::IClassType* AddClassType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid,
                Smp::Uuid baseClassUuid) override {
        return nullptr;
    }
private:
};
TypeReg _typeReg;
// ----------------------------------------------------------
// test suite implementation
ABS_TEST_SUITE_BEGIN( StructureField ) 

private:
    struct DummyStruct {
        Smp::UInt8 fByte;
        Smp::Int64 fInt;
        Smp::Float64 fNumber;
    };
    std::vector<StructureType::FieldDescr> _fields;
    DummyStruct _dst={0x42, 42, 42.42};
    StructureType* _structType;

public:
    void setUp() {
        _structType=new StructureType(Smp::Uuid("e9083d12-60cb-11f0-a109-67b0c3d8bb9d"), &_typeReg, "DummyStructType","",nullptr);
        _fields.push_back({"fByte", "byte field", &_byteType,
                    0, Smp::ViewKind::VK_All, true, true, false});
        _fields.push_back({"fInt", "int field", &_intType,
                    ((uint8_t*)&(_dst.fInt)-(uint8_t*)&_dst), Smp::ViewKind::VK_All, true, true, false});
        _fields.push_back({"fNumber", "number field", &_numberType,
                    ((uint8_t*)&(_dst.fNumber)-(uint8_t*)&_dst), Smp::ViewKind::VK_All, true, true, false});
    }

    void tearDown() {
        delete _structType;
        _fields.clear();
    }

    ABS_TEST_CASE_BEGIN(CreateStructureField) {
        auto  sf=dynamic_cast<Smp::IStructureField*>(StructureField::Create("dummty", "", Smp::ViewKind::VK_All, &_dst, _structType, true, true, false, nullptr, _fields));
        CPPUNIT_ASSERT(sf!=nullptr);
        CPPUNIT_ASSERT_EQUAL((Smp::UInt64)3, sf->GetFields()->size());
        auto f=dynamic_cast<Smp::ISimpleField*>(sf->GetField("fByte"));
        CPPUNIT_ASSERT(f!=nullptr);
        Smp::UInt8 byte=f->GetValue();
        CPPUNIT_ASSERT_EQUAL((Smp::UInt8)0x42, byte);
        f=dynamic_cast<Smp::ISimpleField*>(sf->GetField("fInt"));
        CPPUNIT_ASSERT(f!=nullptr);
        Smp::Int64 integer=f->GetValue();
        CPPUNIT_ASSERT_EQUAL((Smp::Int64)42, integer);
        f=dynamic_cast<Smp::ISimpleField*>(sf->GetField("fNumber"));
        CPPUNIT_ASSERT(f!=nullptr);
        Smp::Float64 number=f->GetValue();
        CPPUNIT_ASSERT_EQUAL((Smp::Float64)42.42, number);
        delete sf;
    }
    ABS_TEST_CASE_END
ABS_TEST_SUITE_END

} // namespace test
