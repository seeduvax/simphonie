/*
 * @file TestContainer.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/smpdk/Container.hpp"
#include "DummyComponent.hpp"

namespace test {
using namespace simph::smpdk;

// ----------------------------------------------------------
// test fixture implementation
class TestContainer: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestContainer );
CPPUNIT_TEST( testContainer );
CPPUNIT_TEST_SUITE_END();

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testContainer() {
        Container c("container");
        DummyComponent c1("one");
        DummyComponent c2("two");
        DummyComponent c2bis("two");
        CPPUNIT_ASSERT_EQUAL((Smp::Int64)0,c.GetCount());
        c.AddComponent(&c1);
        CPPUNIT_ASSERT_EQUAL((Smp::Int64)1,c.GetCount());
        c.AddComponent(&c2);
        CPPUNIT_ASSERT_EQUAL((Smp::Int64)2,c.GetCount());
        try {
            c.AddComponent(&c2bis);
            CPPUNIT_ASSERT_MESSAGE("Adding to the container an entry with an already used name should raise Smp::DuplicateName execption",false);
        }
        catch (Smp::DuplicateName& ex) {
            // This is the expected exception, then OK, continue.
            std::cout << ex.GetMessage() << std::endl;
        }
        CPPUNIT_ASSERT_EQUAL((Smp::Int64)2,c.GetCount());
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestContainer);
} // namespace test
