/*
 * @file TestTimeKeeper.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/kern/TimeKeeper.hpp"
#include "simph/sys/Logger.hpp"

namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestTimeKeeper: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestTimeKeeper );
CPPUNIT_TEST( testTimeKeeper );
CPPUNIT_TEST_SUITE_END();

private:

public:
    void setUp() {
    }

    void tearDown() {
    }
    void testTimeKeeper() {
        TimeKeeper tp;
        Smp::DateTime zulu=tp.GetZuluTime();
        TRACE("zulu="<<(int64_t)zulu);
        tp.reset();
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)0,tp.GetSimulationTime());
        CPPUNIT_ASSERT_EQUAL((Smp::DateTime)0,tp.GetMissionTime());
        CPPUNIT_ASSERT(zulu<tp.GetZuluTime());
        tp.SetMissionTime(100);
        tp.setNextEventTime(10000);
        TRACE("epoch="<<(int64_t)tp.GetEpochTime());
        TRACE("mission="<<(int64_t)tp.GetMissionTime());
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10000,tp.GetSimulationTime());
        CPPUNIT_ASSERT_EQUAL((Smp::DateTime)10100,tp.GetMissionTime());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestTimeKeeper);
} // namespace test
