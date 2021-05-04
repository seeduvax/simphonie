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
#include "simph/kern/Failure.hpp"
#include "simph/smpdk/Collection.hpp"
#include "simph/smpdk/FallibleModel.hpp"

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
