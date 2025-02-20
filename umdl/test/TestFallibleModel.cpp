/*
 * @file TestComposite.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "Smp/IFailure.h"
#include "simdeck/Failure.hpp"
#include "simdeck/Collection.hpp"
#include "simdeck/FallibleModel.hpp"

namespace test {

// ----------------------------------------------------------
// test fixture implementation
class TestFallibleModel : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestFallibleModel);
    CPPUNIT_TEST(testFallibleModel);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void testFallibleModel() {
        // TODO
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestFallibleModel);
}  // namespace test
