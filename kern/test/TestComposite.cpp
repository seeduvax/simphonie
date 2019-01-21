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
#include "simph/kern/Composite.hpp"

namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestComposite: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestComposite );
// TODO for each test method:
// CPPUNIT_TEST( test...);
CPPUNIT_TEST_SUITE_END();

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestComposite);
} // namespace test
