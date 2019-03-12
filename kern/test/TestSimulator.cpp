/*
 * @file TestSimulator.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/kern/Simulator.hpp"
#include "simph/sys/Logger.hpp"

namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestSimulator: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestSimulator );
CPPUNIT_TEST( testStates );
CPPUNIT_TEST_SUITE_END();

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testStates() {
        Simulator sim;
        sim.Publish();
        sim.Configure();
        sim.Connect();
        TRACE(""<<sim.GetTimeKeeper()->GetSimulationTime());
        // TODO schedule something, otherwise simulation time will
        // never change because of the way the scheduler is implemented !
        sim.Run();
        sleep(1);
        sim.Hold();
        TRACE(""<<sim.GetTimeKeeper()->GetSimulationTime());
        sim.Exit();
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestSimulator);
} // namespace test
