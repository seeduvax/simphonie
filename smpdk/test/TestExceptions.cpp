/*
 * @file TESTTOSTRING.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */

#include <string.h>
#include <cppunit/extensions/HelperMacros.h>
#include "Smp/AnySimple.h"
#include "Smp/IObject.h"
#include "Smp/IPublication.h"
#include "Smp/PrimitiveTypes.h"
#include "simph/smpdk/ExInvalidObjectName.hpp"
#include "simph/smpdk/Object.hpp"
//#include "simph/smpdk/ExInvalidParameterValue.hpp"

namespace test {
using namespace simph::smpdk;

// ----------------------------------------------------------
// test fixture implementation
class TestException : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestException);
    CPPUNIT_TEST(testExInvalidParameterValue);
    CPPUNIT_TEST(testExInvalidObjectName);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void testExInvalidParameterValue() {
/* TODO this is dummy a test checking nothing!! Do a real test.
        Smp::Float64 val = 11;
        Smp::AnySimple anysimple;
        anysimple.SetValue(Smp::PrimitiveTypeKind::PTK_Float64, val);
        Smp::IObject* object;
        auto except = simph::kern::ExInvalidParameterValue(object, "InvalidParameterValue test", anysimple);
        std::cout << " • Parameter name : " << except.GetParameterName() << ", value = " << except.GetValue()
                  << std::endl;
*/
    }


    void testExInvalidObjectName() {
        bool exThrown=false;
        try {
            Object o("for","",nullptr);
        }
        catch (Smp::InvalidObjectName& ex) {
            exThrown=true;
            CPPUNIT_ASSERT(strcmp("InvalidObjectName",ex.GetName())==0);
        }
        CPPUNIT_ASSERT(exThrown);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestException);
}  // namespace test
