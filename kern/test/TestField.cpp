/*
 * @file TestField.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/kern/Field.hpp"

namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestField : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestField);
    CPPUNIT_TEST(testInt64);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void testInt64() {
        Smp::Int64 intFValue = 42;
        TField<Smp::Int64> intF("int64", "", Smp::ViewKind::VK_All, &intFValue, true, false, false, nullptr);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestField);
}  // namespace test
