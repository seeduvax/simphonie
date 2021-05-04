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
#include "simph/kern/ExInvalidReturnValue.hpp"

namespace test {

// ----------------------------------------------------------
// test fixture implementation
class TestException : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestException);
    CPPUNIT_TEST(testExInvalidParameterValue);
    CPPUNIT_TEST(testExInvalidReturnValue);
    CPPUNIT_TEST(testExInvalidObjectName);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void testExInvalidParameterValue() {
        Smp::Float64 val = 11;
        Smp::AnySimple anysimple;
        anysimple.SetValue(Smp::PrimitiveTypeKind::PTK_Float64, val);
        Smp::IObject* object;
        auto except = simph::kern::ExInvalidParameterValue(object, "InvalidParameterValue test", anysimple);
        std::cout << " • Parameter name : " << except.GetParameterName() << ", value = " << except.GetValue()
                  << std::endl;
    }

    void testExInvalidReturnValue() {
        Smp::Float64 val = 12;
        Smp::AnySimple anysimple;
        anysimple.SetValue(Smp::PrimitiveTypeKind::PTK_Float64, val);
        Smp::IObject* object;
        auto except = simph::kern::ExInvalidReturnValue(object, "Operation test", anysimple);
        std::cout << " • Name : " << except.GetOperationName() << ", value = " << except.GetValue() << std::endl;
    }

    void testExInvalidObjectName() {
        Smp::IObject* object;
        auto except = simph::kern::ExInvalidObjectName(object, "ObjectName test");
        std::cout << " • Name : " << except.GetInvalidName() << std::endl;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestException);
}  // namespace test
