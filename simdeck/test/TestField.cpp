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
#include "simdeck/Field.hpp"

#include <string>
#include <iostream>

namespace test {
using namespace simdeck;

// ----------------------------------------------------------
// test fixture implementation
class TestField : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestField);
    CPPUNIT_TEST(testInt64);
    CPPUNIT_TEST(testString);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void testInt64() {
        // Smp::Int64 intFValue = 42;
        // TField<Smp::Int64> intF("int64", "", Smp::ViewKind::VK_All, &intFValue, true, false, false, nullptr);
    }

#define TRACE(expr) std::cout << __FILE__ << ":" << __LINE__ << ": " << #expr << "=" << expr << std::endl;
    void testString() {
        std::string s="plop";
        TRACE(sizeof(s));
        TRACE((void*)s.c_str());
        std::string t="bidule skdjfh skdjfh skdjfh skjdfh ksjdfh ksjdfh ksjdfh ksdjfh ksjdhfks jdhfksjdh fksjdh fksjdhfksjd fksjdhfk sjdhfksjdhfksjd hfksjdfh ";
        TRACE(sizeof(t));
        TRACE((void*)t.c_str());
        s=t;
        TRACE(sizeof(s)); 
        TRACE((void*)s.c_str());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestField);
}  // namespace test
