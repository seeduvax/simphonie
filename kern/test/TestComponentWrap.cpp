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
#include "DummyComponent.hpp"
#include "simph/kern/Composite.hpp"

namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestComponentWrap: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestComponentWrap );
CPPUNIT_TEST( testPublish );
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
        DummyComponent one("one");
        ComponentWrap cw(&one,nullptr);
        Composite::dump(std::cout,&cw);
        CPPUNIT_ASSERT(strcmp("one",cw.GetName())==0);
        cw.PublishField("integer","",&intField);
        cw.PublishField("double","",&doubleField);
        Composite::dump(std::cout,&cw);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestComponentWrap);
} // namespace test
