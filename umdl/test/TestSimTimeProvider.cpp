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
#include "Smp/IField.h"
#include "Smp/IDataflowField.h"
#include "simph/kern/Resolver.hpp"
#include "simph/kern/Scheduler.hpp"
#include "simph/kern/Simulator.hpp"
#include "simph/umdl/Logger1D.hpp"
#include "simph/umdl/SimTimeProvider.hpp"
#include "simph/umdl/SysTimeSynchro.hpp"

#include "simph/smpdk/Utils.hpp"
#include "simph/sys/Logger.hpp"

namespace test {
using namespace simph::umdl;

// ----------------------------------------------------------
// test fixture implementation
class TestSimTimeProvider : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestSimTimeProvider);
    CPPUNIT_TEST(testSimTime);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void testSimTime() {
        simph::kern::Simulator sim;
        sim.LoadLibrary("libsimph_umdl.so");
        auto mdl = sim.CreateInstance(simph::smpdk::Utils::generateUuid("SimTimeProvider"), "clock", "", nullptr);
        CPPUNIT_ASSERT(mdl != nullptr);
        auto log = sim.AddModel<Logger1D>("log");
        auto sync = sim.AddModel<SysTimeSynchro>("sync");
        sim.Publish();
        sim.Configure();
        sim.Connect();
        auto f1 = dynamic_cast<Smp::IDataflowField*>(sim.GetResolver()->ResolveRelative("out", mdl));
        CPPUNIT_ASSERT(f1 != nullptr);
        auto f2 = dynamic_cast<Smp::IDataflowField*>(sim.GetResolver()->ResolveRelative("in", log));
        f1->Connect(f2);
        CPPUNIT_ASSERT(f1 != nullptr);
        dynamic_cast<simph::kern::Resolver*>(sim.GetResolver())->dump();
        simph::kern::Scheduler* sched = dynamic_cast<simph::kern::Scheduler*>(sim.GetScheduler());
        sched->AddSimulationTimeEvent(sync->GetEntryPoint("step"),
                                      0,  // 0ms offset
                                      10000000,  // 10ms period
                                      -1);  // for ever
        sched->AddSimulationTimeEvent(dynamic_cast<Smp::IEntryPoint*>(sim.GetResolver()->ResolveRelative("step", mdl)),
                                      15000000,  // 1ms offset
                                      20000000,  // 200ms period
                                      10);  // 10 times only
        sched->AddSimulationTimeEvent(log->GetEntryPoint("step"),
                                      0,  // 0ms offset
                                      40000000,  // 400ms period
                                      -1);  // for ever
        sched->step(1000000000);  // 1sec simulation
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestSimTimeProvider);
}  // namespace test
