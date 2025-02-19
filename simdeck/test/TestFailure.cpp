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
#include "simdeck/Failure.hpp"

namespace test {
using namespace simdeck;

// ----------------------------------------------------------
// test fixture implementation
class TestFailure : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestFailure);
    CPPUNIT_TEST(testFailure);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void testFailure() {
        std::unique_ptr<Failure> failure(new Failure("fail", "failure", nullptr));
        CPPUNIT_ASSERT(failure != nullptr);
        CPPUNIT_ASSERT(failure->IsFailed() == false);
        failure->Fail();
        CPPUNIT_ASSERT(failure->IsFailed() == true);
        failure->Unfail();
        CPPUNIT_ASSERT(failure->IsFailed() == false);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestFailure);
}  // namespace test
