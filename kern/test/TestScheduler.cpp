/*
 * @file TestScheduler.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/kern/Scheduler.hpp"
#include "simph/kern/Simulator.hpp"
#include "simph/sys/Logger.hpp"
#include "simph/sys/Callback.hpp"
#include "simph/sys/ChronoTool.hpp"
#include "simph/kern/EntryPoint.hpp"
#include <unistd.h>
#include <memory>

namespace test {
using namespace simph::kern;
using namespace simph::sys;

// ----------------------------------------------------------
// test fixture implementation
class TestScheduler: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestScheduler );
CPPUNIT_TEST( testSchedule );
CPPUNIT_TEST( testSchedule2 );
CPPUNIT_TEST( testScheduleLongTask );
CPPUNIT_TEST_SUITE_END();

private:
    Simulator* _sim;
    Scheduler* _scheduler;
public:
    void setUp() {
        _sim=new Simulator();
        _scheduler=dynamic_cast<Scheduler*>(_sim->GetScheduler());
        _sim->Publish();
        _sim->Configure();
        _sim->Connect();
    }

    void tearDown() {
        delete _sim;
    }

    void callback(std::vector<Smp::Duration>* vv) {
        auto st = _sim->GetTimeKeeper()->GetSimulationTime();
        vv->push_back(st);
        TRACE(""<<st);
    }

    void testSchedule() {
        std::vector<Smp::Duration> scheduledTime;
        auto cb = Callback::create(&TestScheduler::callback,this,&scheduledTime);
        auto ep = std::make_unique<EntryPoint>(std::move(cb), "callback");

        _scheduler->AddSimulationTimeEvent(ep.get(),10);
        _scheduler->AddSimulationTimeEvent(ep.get(),30);
        _scheduler->AddSimulationTimeEvent(ep.get(),20);
        _scheduler->AddSimulationTimeEvent(ep.get(),20);
        _scheduler->step();
        _scheduler->step();
        _scheduler->step();
        _scheduler->step();

        CPPUNIT_ASSERT_EQUAL((Smp::Duration) 10, scheduledTime[0]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration) 20, scheduledTime[1]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration) 20, scheduledTime[2]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration) 30, scheduledTime[3]);
    }


    void testSchedule2() {
        std::vector<Smp::Duration> scheduledTime;
        auto cb = Callback::create(&TestScheduler::callback,this,&scheduledTime);
        auto ep = std::make_unique<EntryPoint>(std::move(cb), "callback");

        _scheduler->AddSimulationTimeEvent(ep.get(),10);
        _scheduler->AddSimulationTimeEvent(ep.get(),10);
        _scheduler->AddSimulationTimeEvent(ep.get(),10);
        _scheduler->AddSimulationTimeEvent(ep.get(),10);

        _scheduler->step(10);

        CPPUNIT_ASSERT_EQUAL((Smp::Duration) 10, scheduledTime[0]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration) 10, scheduledTime[1]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration) 10, scheduledTime[2]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration) 10, scheduledTime[3]);
    }

    void testScheduleLongTask() {
        auto cb = Callback::create([](){ ::usleep(100000UL);}); // 100 ms sleep
        auto ep = std::make_unique<EntryPoint>(std::move(cb), "callback");

        _scheduler->AddSimulationTimeEvent(ep.get(),10);
        _scheduler->AddSimulationTimeEvent(ep.get(),10);
        _scheduler->AddSimulationTimeEvent(ep.get(),10);
        _scheduler->AddSimulationTimeEvent(ep.get(),10);
        _scheduler->AddSimulationTimeEvent(ep.get(),10);

        auto runTo = Callback::create( [this]() {_scheduler->step();});
        ChronoTool::Record rec = ChronoTool::execution(*runTo);

        auto duration_ms = rec.count<std::chrono::milliseconds>();
        TRACE("recorded duration = " << duration_ms << " ms")
        CPPUNIT_ASSERT( duration_ms - 500 < 20 ) ; // 20ms margin for an expected 500ms execution time
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestScheduler);
} // namespace test
