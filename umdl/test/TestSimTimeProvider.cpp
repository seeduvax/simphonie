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
#include "simph/umdl/Logger1D.hpp"
#include "simph/kern/Simulator.hpp"
#include "simph/kern/Scheduler.hpp"
#include "simph/kern/ObjectsRegistry.hpp"
#include "Smp/IDataflowField.h"

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
        auto mdl=sim.AddModel<SimTimeProvider>("clock");
        auto log=sim.AddModel<Logger1D>("log");
        sim.Publish();
        sim.Configure();
        sim.Connect();
        auto f1=dynamic_cast<Smp::IDataflowField*>(sim.GetResolver()->ResolveRelative("Fields/out",mdl));
        CPPUNIT_ASSERT(f1!=nullptr);
        auto f2=dynamic_cast<Smp::IDataflowField*>(sim.GetResolver()->ResolveRelative("Fields/in",log));
        f1->Connect(f2);
        CPPUNIT_ASSERT(f1!=nullptr);
dynamic_cast<simph::kern::ObjectsRegistry*>(sim.GetResolver())->dump();        
        simph::kern::Scheduler* sched=dynamic_cast<simph::kern::Scheduler*>(sim.GetScheduler());
        sched->AddSimulationTimeEvent(
                mdl->GetEntryPoint("step"),
                15000000, // 1ms offset
                20000000, // 200ms period
                10); // 10 times only
        sched->AddSimulationTimeEvent(
                log->GetEntryPoint("step"),
                0, // 0ms offset
                40000000, // 400ms period
                -1); // for ever
        for (int i=0;i<20;i++) {
            sched->step();
        }
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestSimTimeProvider);
} // namespace test
