/*
 * @file TestIncrement.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/kern/Simulator.hpp"
#include "simph/sys/Logger.hpp"
#include "simph/kern/Scheduler.hpp"

#include "SmpIncrement.hpp"


namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestIncrement: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestIncrement);
CPPUNIT_TEST( testIncrement );
CPPUNIT_TEST_SUITE_END();

private:
    Simulator* _sim;
    Scheduler* _scheduler;

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testIncrement() {
        sim=new Simulator();
        
        SmpIncrement* increment = new SmpIncrement(0);
        _scheduler = dynamic_cast<Scheduler*>(sim.GetScheduler());

        auto increment=new SmpIncrement("increment","increment",&sim);
        sim.AddModel(increment);

        sim.Publish();
        sim.Configure();
        sim.Connect();

        auto input = dynamic_cast<Smp::IDataflowField*>(sim.GetResolver()->ResolveRelative("input",increment));
        auto output = dynamic_cast<Smp::IDataflowField*>(sim.GetResolver()->ResolveRelative("output",increment));

        output->connect(input);

        _scheduler.AddSimulationTimeEvent(
            increment.increment(increment->GetEntryPoint("step")),
            0, // 0ms offset
            1000000, // 1000000ns period
            10); // 10 times
        
        sim.Run();
        _scheduler->step(10000000);

        CPPUNIT_ASSERT_EQUAL(input,10);
        CPPUNIT_ASSERT_EQUAL(output,11);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestIncrement);
} // namespace test
