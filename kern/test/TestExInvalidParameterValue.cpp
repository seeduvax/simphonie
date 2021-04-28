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
#include "Smp/PrimitiveTypes.h"
#include "simph/kern/ExInvalidParameterValue.hpp"

namespace test {

// ----------------------------------------------------------
// test fixture implementation
class TestExInvalidParameterValue : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestExInvalidParameterValue);
    CPPUNIT_TEST(testExInvalidParameterValue);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void testExInvalidParameterValue() {
        Smp::Float64 val = 12;
        Smp::AnySimple anysimple;
        anysimple.SetValue(Smp::PrimitiveTypeKind::PTK_Float64, val);
        Smp::IObject* object;
        simph::kern::ExInvalidParameterValue(object, "test", anysimple);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestExInvalidParameterValue);
}  // namespace test
