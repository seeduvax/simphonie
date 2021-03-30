/*
 * @file TestSmpIncrement.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/T_kern/SmpIncrement.hpp"
#include "simph/kern/Simulator.hpp"
#include "simph/kern/Field.hpp"
#include "simph/sys/Logger.hpp"
#include "simph/kern/Scheduler.hpp"
#include "Smp/IDataflowField.h"


namespace test {
using namespace simph::T_kern;
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestSmpIncrement: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestSmpIncrement );
// TODO for each test method:
CPPUNIT_TEST( testIncrement );
CPPUNIT_TEST_SUITE_END();
public:
    void setUp() {
    }

    void tearDown() {
    }

    void testIncrement() {
        Simulator sim;
        
        SmpIncrement* increment = new SmpIncrement("increment","increment",nullptr);
        auto scheduler = dynamic_cast<Scheduler*>(sim.GetScheduler());

        //auto increment=new SmpIncrement("increment","increment",&_sim);
        sim.AddModel(increment);

        sim.Publish();
        sim.Configure();
        sim.Connect();

        auto input = dynamic_cast<Field*>(sim.GetResolver()->ResolveRelative("input",increment));
        auto output = dynamic_cast<Field*>(sim.GetResolver()->ResolveRelative("output",increment));

        output->Connect(input);

        scheduler->AddSimulationTimeEvent(
            //increment->SmpIncrement(increment->GetEntryPoint("step")),
            increment->GetEntryPoint("step"),
            0, // 0ms offset
            1000000, // 1000000ns period
            -1); // 

        scheduler->step(10000000);

        CPPUNIT_ASSERT_EQUAL((double) 11, input->GetValue().value.float64Value);
        CPPUNIT_ASSERT_EQUAL((double) 11, output->GetValue().value.float64Value);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestSmpIncrement);
} // namespace test
