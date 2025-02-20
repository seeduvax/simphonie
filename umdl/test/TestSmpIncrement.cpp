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
#include "Smp/IOutputField.h"
#include "Smp/ISimpleField.h"
#include "simphonie/kern/Resolver.hpp"
#include "simphonie/kern/Scheduler.hpp"
#include "simphonie/kern/Simulator.hpp"
#include "simdeck/Utils.hpp"
#include "simphonie/sys/Logger.hpp"
#include "simphonie/umdl/SmpIncrement.hpp"
#include "SimControlEnd.hpp"

namespace test {
using namespace simphonie::umdl;
using namespace simphonie::kern;

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
/*
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
*/
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
            sim.LoadLibrary("simph_umdl");
            increment = dynamic_cast<Smp::IEntryPointPublisher*>(sim.CreateInstance(
                simdeck::Utils::generateUuid("SmpIncrement"), "increment", "description Increment", &sim));
        }
        CPPUNIT_ASSERT(increment != nullptr);

        sim.Publish();
        sim.Configure();
        sim.Connect();

        auto input = dynamic_cast<Smp::ISimpleField*>(sim.GetResolver()->ResolveAbsolute("increment.input"));
        CPPUNIT_ASSERT(input != nullptr);
        auto output = dynamic_cast<Smp::IOutputField*>(sim.GetResolver()->ResolveAbsolute("increment.output"));
        CPPUNIT_ASSERT(output != nullptr);
        auto step = dynamic_cast<Smp::IEntryPoint*>(sim.GetResolver()->ResolveAbsolute("increment.step"));
        CPPUNIT_ASSERT(step != nullptr);

        output->Connect(input);
        scheduler->AddSimulationTimeEvent(step,
                                          0,  // 0ms offset
                                          1000000,  // 1000000ns period
                                          -1);  //
        
        SimControlEnd simCtl(&sim,10000000);
        sim.Run();
        simCtl.wait();

        CPPUNIT_ASSERT_EQUAL((double)11, input->GetValue().value.float64Value);
        CPPUNIT_ASSERT_EQUAL((double)11, dynamic_cast<Smp::ISimpleField*>(output)->GetValue().value.float64Value);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestSmpIncrement);
}  // namespace test
