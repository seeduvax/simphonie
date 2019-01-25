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
#include "simph/kern/Container.hpp"
#include "DummyComponent.hpp"

namespace test {
using namespace simph::kern;

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
        Container c;
        DummyComponent c1("one");
        DummyComponent c2("two");
        DummyComponent c2bis("two");
        CPPUNIT_ASSERT_EQUAL(0,c.GetCount());
        c.AddComponent(&c1);
        CPPUNIT_ASSERT_EQUAL(1,c.GetCount());
        c.AddComponent(&c2);
        CPPUNIT_ASSERT_EQUAL(2,c.GetCount());
        try {
            c.AddComponent(&c2bis);
            CPPUNIT_ASSERT_MESSAGE("Adding to the container an entry with a already used name should raise Smp::DuplicateName execption",false)
        }
        catch (Smp::DuplicateName ex) {
        }
        catch (...) {
            CPPUNIT_ASSERT_MESSAGE("Exception of unexpected type thrown, expected is Smp::DuplicateName",false)
        }        
        CPPUNIT_ASSERT_EQUAL(2,c.GetCount());
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestContainer);
} // namespace test
