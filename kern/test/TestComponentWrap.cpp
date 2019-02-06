/*
 * @file TestComponentWrap.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/kern/ComponentWrap.hpp"
#include "simph/sys/Logger.hpp"

namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestComponentWrap: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestComponentWrap );
// TODO for each test method:
// CPPUNIT_TEST( test...);
CPPUNIT_TEST_SUITE_END();

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testPublish() {
        Smp::Int32 intField=12;
        Smp::Float64 doubleField=42;
        ComponentWrap cw(nullptr);
//        cw.dump()
        TRACE("")
        cw.PublishField("integer","",&intField);
        cw.PublishField("double","",&doubleField);
//        cw.dump()
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestComponentWrap);
} // namespace test
