/*
 * @file TestField.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "abs/test.h"
#include "simdeck/SimpleType.hpp"
#include "Smp/IOutputField.h"

#include <string>
#include <iostream>

namespace test {
using namespace simdeck;

    static SimpleType _int64Type(Smp::Uuids::Uuid_Int64, Smp::PrimitiveTypeKind::PTK_Int64, "Int64",
                       "Eight bytes signed int data type");

// ----------------------------------------------------------
// test fixture implementation
ABS_TEST_SUITE_BEGIN( Field )

private:

public:
    void setUp() {}

    void tearDown() {}

    ABS_TEST_CASE_BEGIN( Int64 ) {
        Smp::Int64 intFValue = 42;
        auto intF=SimpleField::Create("int64", "", Smp::ViewKind::VK_All, &_int64Type, &intFValue, true, false, false, nullptr);
        auto v=intF->GetValue();
        CPPUNIT_ASSERT_EQUAL((Smp::Int64)42,(Smp::Int64)v);
        intFValue=43;
        v=intF->GetValue();
        CPPUNIT_ASSERT_EQUAL((Smp::Int64)43,(Smp::Int64)v);
        delete intF;
    }
    ABS_TEST_CASE_END

    ABS_TEST_CASE_BEGIN( Int64Push ) {
        Smp::Int64 inFValue = 42;
        Smp::Int64 outFValue = 43;
        auto inF=SimpleField::Create("in", "", Smp::ViewKind::VK_All, &_int64Type, &inFValue, false, true, false, nullptr);
        auto outF=SimpleField::Create("out", "", Smp::ViewKind::VK_All, &_int64Type, &outFValue, false, false, true, nullptr);
        auto out=dynamic_cast<Smp::IOutputField*>(outF);
        CPPUNIT_ASSERT(out!=nullptr);
        out->Connect(inF);

        CPPUNIT_ASSERT_EQUAL((Smp::Int64)42,(Smp::Int64)inF->GetValue());
        CPPUNIT_ASSERT_EQUAL((Smp::Int64)43,(Smp::Int64)outF->GetValue());
        for (uint64_t i=0; i<10000000; i++) {
            // This is done many times just to get some measurement
            // of the performance. The absolute value is not meaningful
            // this is just to see when implementation change brings
            // performance or not.
            // Initial Push implementaiton was palying with dynamic_cast 
            // resulting in more than 2sec for this test. Introduction of
            // smarter FieldCopy (SimpleField.cpp internal class) let it run
            // faster, less than 0.5ms, running on the same machine.
            out->Push();
        }

        CPPUNIT_ASSERT_EQUAL((Smp::Int64)43,(Smp::Int64)inF->GetValue());
        CPPUNIT_ASSERT_EQUAL((Smp::Int64)43,(Smp::Int64)outF->GetValue());

        out->Disconnect(inF);
        
        outFValue=44;
        out->Push();
        CPPUNIT_ASSERT_EQUAL((Smp::Int64)44,(Smp::Int64)outF->GetValue());
        // no more propagate value change after disconnect.
        CPPUNIT_ASSERT_EQUAL((Smp::Int64)43,(Smp::Int64)inF->GetValue());

        delete outF;
        delete inF;
    }
    ABS_TEST_CASE_END

ABS_TEST_SUITE_END
}  // namespace test
