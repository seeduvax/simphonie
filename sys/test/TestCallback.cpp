/*
 * @file TestCallback.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/sys/Callback.hpp"

namespace test {
using namespace simph::sys;

// ----------------------------------------------------------
// test fixture implementation
class TestCallback : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestCallback);
    // TODO for each test method:
    CPPUNIT_TEST(testSimpleCallback);
    CPPUNIT_TEST(testLambdaCallback);
    CPPUNIT_TEST_SUITE_END();

private:
    bool _value;

    void setValue(bool newValue) {
        _value = newValue;
    }

public:
    void setUp() {}

    void tearDown() {}

    void testSimpleCallback() {
        _value = false;
        auto cb = Callback::create(&TestCallback::setValue, this, true);
        cb->run();
        CPPUNIT_ASSERT(_value);
    }

    void testLambdaCallback() {
        bool refValue = false;
        auto cb = Callback::create([&refValue](bool newValue) { refValue = newValue; },
                                   true);  // newVal
        cb->run();
        CPPUNIT_ASSERT(refValue);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestCallback);
}  // namespace test
