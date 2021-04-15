/*
 * @file TestLogger.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/sys/Logger.hpp"

namespace test {
using namespace simph::sys;

// ----------------------------------------------------------
// test fixture implementation
class TestLogger : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestLogger);
    CPPUNIT_TEST(testLogger);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void testLogger() {
        TRACE("Trace message")
        LOGD("Debug message " << 42)
        LOGI("Info message")
        LOGW("Warning message")
        LOGE("Error message")
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestLogger);
}  // namespace test
