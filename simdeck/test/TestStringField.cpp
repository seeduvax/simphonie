/*
 * @file TestStringType.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "abs/test.h"
#include "simdeck/StringField.hpp"

namespace test {
using namespace simdeck;

#define DUMMY_STR "Dummy string value"
#define DUMMY_STR_BIS "2nd dummy string value"
// ----------------------------------------------------------
// test suite implementation
ABS_TEST_SUITE_BEGIN( StringField )
// uncomment and complete next line for test suite description
// ABS_TEST_DESCR(test description)

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

    ABS_TEST_CASE_BEGIN(SimpleTypeAssign) {
        std::string str=DUMMY_STR;
        StringField* sf=StringField::Create("testsrt", "", 
                                Smp::ViewKind::VK_All, &str, nullptr,
                                false, true, false, nullptr);
        CPPUNIT_ASSERT( sf!=nullptr );
        auto value=sf->GetValue();

        CPPUNIT_ASSERT_EQUAL(Smp::PrimitiveTypeKind::PTK_String8, sf->GetType()->GetPrimitiveTypeKind());
        Smp::Uuid expUuid={ 0, 0, 0, { ' ',' ','S','t','r','i','n','g'} };
        CPPUNIT_ASSERT_EQUAL(expUuid, sf->GetType()->GetUuid());
        std::string exp=DUMMY_STR;
        std::string vstr=(Smp::String8)value;
        CPPUNIT_ASSERT_EQUAL(exp, vstr);
        Smp::AnySimple simple(Smp::PrimitiveTypeKind::PTK_String8,DUMMY_STR_BIS);
        sf->SetValue(simple);
        exp=DUMMY_STR_BIS;
        CPPUNIT_ASSERT_EQUAL(exp, str);
        delete sf;
    }
    ABS_TEST_CASE_END
ABS_TEST_SUITE_END
} // namespace test
