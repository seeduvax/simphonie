/*
 * @file TESTTOSTRING.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "Smp/AnySimple.h"
#include "Smp/IObject.h"
#include "Smp/IPublication.h"
#include "Smp/PrimitiveTypes.h"
#include "simph/kern/ExInvalidObjectName.hpp"
#include "simph/kern/ExInvalidParameterValue.hpp"

namespace test {

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
/* TODO this is dummy a test checking nothing!! Do a real test.
        Smp::IObject* object;
        auto except = simph::kern::ExInvalidObjectName(object, "ObjectName test");
        std::cout << " • Name : " << except.GetInvalidName() << std::endl;
*/
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestException);
}  // namespace test
