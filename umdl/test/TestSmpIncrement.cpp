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
#include "Smp/IField.h"
#include "simph/kern/Field.hpp"
#include "simph/kern/Resolver.hpp"
#include "simph/kern/Scheduler.hpp"
#include "simph/kern/Simulator.hpp"
#include "simph/smpdk/Utils.hpp"
#include "simph/sys/Logger.hpp"
#include "simph/umdl/SmpIncrement.hpp"
#include "sol/sol.hpp"

namespace test {
using namespace simph::umdl;
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestSmpIncrement : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestSmpIncrement);
    CPPUNIT_TEST(testLifeCycle);
    CPPUNIT_TEST(testLifeCycleWithDynamicLoad);
    // CPPUNIT_TEST(testLifeCycleFromLua);
    // CPPUNIT_TEST(testLifeCycleFromHighLevelLua);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void testLifeCycleFromLua() {
        sol::state lua;
        lua.open_libraries(sol::lib::base, sol::lib::package);
        lua.script_file("test/lua/sim_runner.lua");
        // sol::load_result r = lua.script("test/lua/sim_runner.lua");
        // sol::protected_function_result exec_r = r();
        // CPPUNIT_ASSERT(exec_r.valid());
    }

    void testLifeCycleFromHighLevelLua() {
        sol::state lua;
        lua.open_libraries(sol::lib::base, sol::lib::package);
        lua.script_file("test/lua/sim_runner_high_level.lua");
    }

    void testLifeCycle() {
        testIncrement(false);
    }

    void testLifeCycleWithDynamicLoad() {
        testIncrement(true);
    }

    void testIncrement(bool useDynamicLoad) {
        Simulator sim;
        auto scheduler = dynamic_cast<Scheduler*>(sim.GetScheduler());

        Smp::IEntryPointPublisher* increment;

        if (!useDynamicLoad) {
            auto instance = new SmpIncrement("increment", "increment", nullptr);
            sim.AddModel(instance);
            increment = instance;
        }
        else {
            sim.LoadLibrary("libsimph_umdl.so");
            increment = dynamic_cast<Smp::IEntryPointPublisher*>(sim.CreateInstance(
                simph::smpdk::Utils::generateUuid("SmpIncrement"), "increment", "description Increment", &sim));
        }
        CPPUNIT_ASSERT(increment != nullptr);

        sim.Publish();
        sim.Configure();
        sim.Connect();

        auto input = dynamic_cast<Field*>(sim.GetResolver()->ResolveAbsolute("increment.input"));
        CPPUNIT_ASSERT(input != nullptr);
        auto output = dynamic_cast<Field*>(sim.GetResolver()->ResolveAbsolute("increment.output"));
        CPPUNIT_ASSERT(output != nullptr);
        auto step = dynamic_cast<EntryPoint*>(sim.GetResolver()->ResolveAbsolute("increment.step"));
        CPPUNIT_ASSERT(step != nullptr);

        output->Connect(input);
        scheduler->AddSimulationTimeEvent(step,
                                          0,  // 0ms offset
                                          1000000,  // 1000000ns period
                                          -1);  //
        scheduler->step(10000000);

        CPPUNIT_ASSERT_EQUAL((double)11, input->GetValue().value.float64Value);
        CPPUNIT_ASSERT_EQUAL((double)11, output->GetValue().value.float64Value);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestSmpIncrement);
}  // namespace test
