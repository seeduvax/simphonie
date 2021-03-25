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
#include "simph/kern/Field.hpp"
#include "simph/sys/Logger.hpp"
#include "simph/kern/Scheduler.hpp"
#include "Smp/IDataflowField.h"

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
        _sim=new Simulator();
        
        SmpIncrement* increment = new SmpIncrement("increment","increment",nullptr);
        _scheduler = dynamic_cast<Scheduler*>(_sim->GetScheduler());

        //auto increment=new SmpIncrement("increment","increment",&_sim);
        _sim->AddModel(increment);


        _sim->Publish();
        _sim->Configure();
        _sim->Connect();

        auto input = dynamic_cast<Field*>(_sim->GetResolver()->ResolveRelative("input",increment));
        auto output = dynamic_cast<Field*>(_sim->GetResolver()->ResolveRelative("output",increment));

        output->Connect(input);

        _scheduler->AddSimulationTimeEvent(
            //increment->SmpIncrement(increment->GetEntryPoint("step")),
            increment->GetEntryPoint("step"),
            0, // 0ms offset
            1000000, // 1000000ns period
            10); // 10 times
        
        _sim->Run();
        _scheduler->step(10000000);
        
        CPPUNIT_ASSERT_EQUAL((double) 10, output->GetValue().value.float64Value);
        //CPPUNIT_ASSERT_EQUAL(11, output->GetValue());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestIncrement);
} // namespace test
