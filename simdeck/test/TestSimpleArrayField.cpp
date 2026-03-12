/*
 * @file TestSimpleArrayField.cpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "abs/test.h"
#include "simdeck/SimpleArrayField.hpp"
#include "simdeck/ArrayType.hpp"
#include "simdeck/SimpleType.hpp"
 
namespace test {
using namespace simdeck;
#define ARRAY_SIZE 6
 
 // ----------------------------------------------------------
 // test suite implementation
ABS_TEST_SUITE_BEGIN( SimpleArrayField )
 // uncomment and complete next line for test suite description
 // ABS_TEST_DESCR(test description)
 
private:
    Smp::Uuid _uuidItem;
    Smp::Uuid _uuidArray;
    SimpleType* _itemType;
    ArrayType* _arrayType;
 
public:
    void setUp() {
        _itemType = new SimpleType(_uuidItem, Smp::PrimitiveTypeKind::PTK_Int32, "integer", "", nullptr);
        _arrayType = new ArrayType(_uuidArray, "aint", "", nullptr, ARRAY_SIZE, _itemType);
    }
     
    void tearDown() {
        delete _arrayType;
        delete _itemType;
    }
     
    ABS_TEST_CASE_BEGIN(ArrayWrapping) {
        Smp::Int32 aint[]={1,2,4,8,16,32};
        auto af=SimpleArrayField::Create("aint","", ARRAY_SIZE, aint, _itemType,
                Smp::ViewKind::VK_All, _arrayType,
                false, true, false, nullptr);
        CPPUNIT_ASSERT_EQUAL((Smp::UInt64)ARRAY_SIZE, af->GetSize());
        auto v=af->GetValue(2);
        CPPUNIT_ASSERT_EQUAL(aint[2],(Smp::Int32)v);
        aint[2]=42;
        v=af->GetValue(2);
        CPPUNIT_ASSERT_EQUAL(aint[2],(Smp::Int32)v);

    }
    ABS_TEST_CASE_END

ABS_TEST_SUITE_END
} // namespace test
