/*
 * @file TestSampler.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simphonie/esmp/Sampler.hpp"
#include "simphonie/kern/Resolver.hpp"
#include "simphonie/kern/Scheduler.hpp"
#include "simphonie/kern/Simulator.hpp"
#include "simdeck/Utils.hpp"
#include "simphonie/sys/Logger.hpp"
#include "simphonie/umdl/SmpIncrement.hpp"
#include "SimControlEnd.hpp"
#include "Smp/IOutputField.h"

namespace test {
using namespace simphonie::umdl;
using namespace simphonie::kern;
using namespace simphonie::sys;

// ----------------------------------------------------------
// test fixture implementation
class TestSampler : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestSampler);
    CPPUNIT_TEST(testSampler);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void endSimuCtrl(Smp::ISimulator* sim, Smp::Duration simTime) {
        if (sim->GetTimeKeeper()->GetSimulationTime()>simTime) {
            TRACE("Requesting simulation end");
            sim->Hold(true);
        }
    }

    void testSampler() {
        Simulator sim;
        auto scheduler = dynamic_cast<Scheduler*>(sim.GetScheduler());
        auto resolver = dynamic_cast<Resolver*>(sim.GetResolver());
/* TODO delete or upgrade and restore. Why was this introduced in this test?
        simdeck::Component obj1("obj1", "test obj 1", &sim);
        Smp::IPublication* q = resolver->publish(&obj1);
        CPPUNIT_ASSERT(q != nullptr);

        Smp::Int32 iArray[] = {12, 17, 42};
        q->PublishArray("iArray", "test int array", 3, iArray, Smp::PrimitiveTypeKind::PTK_Int32);

*/
        auto increment = new SmpIncrement("increment", "increment", &sim);
        sim.AddModel(increment);
        // auto sampler = new Sampler("sampler", "description sampler", &sim);
        // sim.AddModel(sampler);

        sim.LoadLibrary("simph_kern");
        auto sampler = dynamic_cast<simphonie::kern::Sampler*>(
            sim.CreateInstance(simdeck::Utils::GenerateUuid("Sampler"), "sampler", "description sampler", &sim));

        sim.Publish();
        sim.Configure();

        auto input = dynamic_cast<Field*>(sim.GetResolver()->ResolveAbsolute("increment.input"));
        CPPUNIT_ASSERT(input != nullptr);
        auto output = dynamic_cast<Smp::IOutputField*>(sim.GetResolver()->ResolveAbsolute("increment.output"));
        CPPUNIT_ASSERT(output != nullptr);
/*
        auto arrayfield = dynamic_cast<Field*>(sim.GetResolver()->ResolveAbsolute("obj1.iArray"));
        CPPUNIT_ASSERT(arrayfield != nullptr);
*/
        auto stepIncrement = dynamic_cast<EntryPoint*>(sim.GetResolver()->ResolveAbsolute("increment.step"));
        CPPUNIT_ASSERT(stepIncrement != nullptr);
        output->Connect(input);

        auto stepSampler = dynamic_cast<EntryPoint*>(sim.GetResolver()->ResolveAbsolute("sampler.step"));
        CPPUNIT_ASSERT(stepSampler != nullptr);
/* TOBE restored. And reconsider sampler API to rely on "standard" SMP API to
 * perform connection if achievable.
        sampler->recordField(input);
        sampler->recordField(output);
        sampler->recordField(arrayfield);
*/
        sim.Connect();

        scheduler->AddSimulationTimeEvent(stepIncrement,
                                          0,  // 0ms offset
                                          1000000,  // 1000000ns period
                                          -1);  //
        scheduler->AddSimulationTimeEvent(stepSampler,
                                          0,  // 0ms offset
                                          1000000,  // 1000000ns period
                                          -1);  //
        SimControlEnd simCtl(&sim,10000000);
        sim.Run();
        simCtl.wait();

        // TODO some asserts are defintely needed here to check everything
        // is OK.

        // sim.find<Smp::IEntryPoint>("sampler.debugPrintFile")->Execute();
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestSampler);
}  // namespace test
