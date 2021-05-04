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
#include "simph/kern/Scheduler.hpp"
#include "simph/kern/Simulator.hpp"
#include "simph/sys/Logger.hpp"
#include "simph/umdl/Sampler.hpp"
#include "simph/umdl/SmpIncrement.hpp"

namespace test {
using namespace simph::umdl;
using namespace simph::kern;

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

    void testSampler() {
        Simulator sim;
        auto scheduler = dynamic_cast<Scheduler*>(sim.GetScheduler());

        auto increment = new SmpIncrement("increment", "increment", &sim);
        sim.AddModel(increment);
        auto sampler = new Sampler("sampler", "description sampler", &sim, "sampler");
        sim.AddModel(sampler);

        sim.Publish();
        sim.Configure();
        sim.Connect();

        auto input = dynamic_cast<Field*>(sim.GetResolver()->ResolveAbsolute("increment.input"));
        CPPUNIT_ASSERT(input != nullptr);
        auto output = dynamic_cast<Field*>(sim.GetResolver()->ResolveAbsolute("increment.output"));
        CPPUNIT_ASSERT(output != nullptr);
        auto stepIncrement = dynamic_cast<EntryPoint*>(sim.GetResolver()->ResolveAbsolute("increment.step"));
        CPPUNIT_ASSERT(stepIncrement != nullptr);
        output->Connect(input);

        auto stepSampler = dynamic_cast<EntryPoint*>(sim.GetResolver()->ResolveAbsolute("sampler.step"));
        CPPUNIT_ASSERT(stepSampler != nullptr);
        sampler->AddField(input);
        sampler->AddField(output);

        scheduler->AddSimulationTimeEvent(stepIncrement,
                                          0,  // 0ms offset
                                          1000000,  // 1000000ns period
                                          -1);  //
        scheduler->AddSimulationTimeEvent(stepSampler,
                                          0,  // 0ms offset
                                          1000000,  // 1000000ns period
                                          -1);  //
        scheduler->step(10000000);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestSampler);
}  // namespace test
