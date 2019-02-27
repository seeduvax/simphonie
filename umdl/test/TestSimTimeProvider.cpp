/*
 * @file TestSimTimeProvider.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/umdl/SimTimeProvider.hpp"
#include "simph/kern/Simulator.hpp"
#include "simph/kern/Scheduler.hpp"
#include "simph/kern/ObjectsRegistry.hpp"

namespace test {
using namespace simph::umdl;

// ----------------------------------------------------------
// test fixture implementation
class TestSimTimeProvider: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestSimTimeProvider );
CPPUNIT_TEST( testSimTime );
CPPUNIT_TEST_SUITE_END();

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testSimTime() {
        simph::kern::Simulator sim;
        SimTimeProvider* mdl=new SimTimeProvider("clock","",sim.GetContainer("Models"));
        sim.AddModel(mdl);
        sim.Publish();
        sim.Configure();
        sim.Connect();
dynamic_cast<simph::kern::ObjectsRegistry*>(sim.GetResolver())->dump();        
        simph::kern::Scheduler* sched=dynamic_cast<simph::kern::Scheduler*>(sim.GetScheduler());
        sched->AddSimulationTimeEvent(
                mdl->GetEntryPoint("step"),
                15000000, // 1ms offset
                20000000, // 200ms period
                10);
        for (int i=0;i<20;i++) {
            sched->step();
        }
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestSimTimeProvider);
} // namespace test
