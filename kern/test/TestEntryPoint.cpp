/*
 * @file TestEntryPoint.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/kern/EntryPoint.hpp"
#include "simph/sys/Callback.hpp"

namespace test {
using namespace simph::kern;
using namespace simph::sys;

// ----------------------------------------------------------
// test fixture implementation
class TestEntryPoint: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestEntryPoint );
// TODO for each test method:
 CPPUNIT_TEST( testCreation);
CPPUNIT_TEST_SUITE_END();

private:
    bool _value;

    void setValue(bool newValue) {
        _value = newValue;
    }

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testCreation() {
        auto cb = Callback::create(&TestEntryPoint::setValue, this, true);
        EntryPoint ep(std::move(cb),"ep");
        ep.Execute();
        CPPUNIT_ASSERT(_value);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestEntryPoint);
} // namespace test
