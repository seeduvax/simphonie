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
#include "Smp/IOutputField.h"
#include "simphonie/kern/Resolver.hpp"
#include "simphonie/kern/Scheduler.hpp"
#include "simphonie/kern/Simulator.hpp"
#include "simphonie/umdl/Logger1D.hpp"
#include "simphonie/umdl/SimTimeProvider.hpp"
#include "simphonie/umdl/SysTimeSynchro.hpp"
#include "simdeck/front/SimFront.hpp"

#include "simdeck/Utils.hpp"
#include "simphonie/sys/Logger.hpp"

namespace test {
using namespace simphonie::umdl;

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
        simphonie::kern::Simulator sim;
        sim.LoadLibrary("simphonie_umdl");
        auto mdl = sim.CreateInstance(simdeck::Utils::GenerateUuid("SimTimeProvider"), "clock", "", nullptr);
        CPPUNIT_ASSERT(mdl != nullptr);
        auto log = new Logger1D("log","",&sim);
        sim.AddModel(log);
        auto sync = new SysTimeSynchro("sync","",&sim);
        sim.AddModel(sync);
        sim.Publish();
        sim.Configure();
        sim.Connect();
        auto f1 = dynamic_cast<Smp::IOutputField*>(sim.GetResolver()->ResolveRelative("out", mdl));
        CPPUNIT_ASSERT(f1 != nullptr);
        auto f2 = dynamic_cast<Smp::IField*>(sim.GetResolver()->ResolveRelative("in", log));
        CPPUNIT_ASSERT(f2 != nullptr);
        f1->Connect(f2);
        dynamic_cast<simphonie::kern::Resolver*>(sim.GetResolver())->dump();
        simphonie::kern::Scheduler* sched = dynamic_cast<simphonie::kern::Scheduler*>(sim.GetScheduler());
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
        simdeck::front::SimFront front(&sim); 
        front.SetEndSimulationTime(1000000000); // 1sec simulation
        front.Run();
        CPPUNIT_ASSERT(front.Wait(Smp::SimulatorStateKind::SSK_Standby, 10000000000L));
        // TODO some asserts here.
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestSimTimeProvider);
}  // namespace test
