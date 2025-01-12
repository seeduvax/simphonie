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
#include <unistd.h>
#include <memory>
#include "simph/kern/EntryPoint.hpp"
#include "simph/kern/Scheduler.hpp"
#include "simph/kern/Simulator.hpp"
#include "simph/sys/Callback.hpp"
#include "simph/sys/ChronoTool.hpp"
#include "simph/sys/Logger.hpp"

namespace test {
using namespace simph::kern;
using namespace simph::sys;

// ----------------------------------------------------------
// test fixture implementation
class TestScheduler : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestScheduler);
    CPPUNIT_TEST(testSchedule);
    CPPUNIT_TEST(testSchedule2);
    CPPUNIT_TEST(testScheduleLongTask);
    CPPUNIT_TEST_SUITE_END();

private:
    class StopControl: public Object {
    public:
        StopControl(std::mutex& m, bool& r, std::condition_variable& cv): 
                Object("","",nullptr), _mutex(m), _run(r), _monitor(cv) 
        {
        } 
        virtual ~StopControl() {}
        void epLeaveExecuting() {
            {
                Synchronized(_mutex);
                _run=false;
            }
            MonitorNotifyAll(_monitor);
        }
        std::mutex& _mutex;
        bool& _run;
        std::condition_variable& _monitor;
    };
    Simulator* _sim;
    Scheduler* _scheduler;
    std::mutex _mutex;
    std::condition_variable _monitor;
    Smp::IEntryPoint* _epLeaveExecuting;
    bool _run=false;

public:
    void setUp() {
        _sim = new Simulator();
        _sim->Publish();
        _sim->Configure();
        _sim->Connect();
        _scheduler = dynamic_cast<Scheduler*>(_sim->GetScheduler());
        StopControl sc(_mutex, _run, _monitor);
        _epLeaveExecuting=EntryPoint::Create("leaveExecution","",&sc,
                &StopControl::epLeaveExecuting);
        _sim->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_LeaveExecutingId,_epLeaveExecuting);
    }

    void tearDown() {
        delete _epLeaveExecuting;
        delete _sim;
    }

    void simStart() {
        {
            Synchronized(_mutex);
            _run=true;
        }
        _sim->Run();
    }

    void waitSimEnd() {
        Synchronized(_mutex);
        while (_run!=false) {
            MonitorWait(_monitor);
        }
    }


    void callback(std::vector<Smp::Duration>* vv) {
        auto st = _sim->GetTimeKeeper()->GetSimulationTime();
        vv->push_back(st);
        TRACE("" << st);
        if (vv->size()>=4) {
            // emulate simulator stop after 4 iterations
            _sim->Hold(true);
        }
    }

    void testSchedule() {
        std::vector<Smp::Duration> scheduledTime;
        auto cb = Callback::create(&TestScheduler::callback, this, &scheduledTime);
        auto ep = std::make_unique<EntryPoint>(std::move(cb), "callback");

        _scheduler->AddSimulationTimeEvent(ep.get(), 10);
        _scheduler->AddSimulationTimeEvent(ep.get(), 30);
        _scheduler->AddSimulationTimeEvent(ep.get(), 20);
        _scheduler->AddSimulationTimeEvent(ep.get(), 20);
        simStart();
        waitSimEnd();
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[0]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)20, scheduledTime[1]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)20, scheduledTime[2]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)30, scheduledTime[3]);
    }

    void endSimuCtrl(Smp::Duration simTime) {
        if (_sim->GetTimeKeeper()->GetSimulationTime()>simTime) {
            TRACE("Requesting simulation end");
            _sim->Hold(true);
        }
    }
    void testSchedule2() {
        std::vector<Smp::Duration> scheduledTime;
        auto cb = Callback::create(&TestScheduler::callback, this, &scheduledTime);
        auto epcb = std::make_unique<EntryPoint>(std::move(cb), "callback");

        _scheduler->AddSimulationTimeEvent(epcb.get(), 10);
        _scheduler->AddSimulationTimeEvent(epcb.get(), 10);
        _scheduler->AddSimulationTimeEvent(epcb.get(), 10);
        _scheduler->AddSimulationTimeEvent(epcb.get(), 10);
        _scheduler->AddSimulationTimeEvent(epcb.get(), 20);

        Smp::Duration stopTime=10;
        auto f = Callback::create(&TestScheduler::endSimuCtrl, this, stopTime);
        auto epf=std::make_unique<EntryPoint>(std::move(f),"endSimuCtrl");
        _sim->GetEventManager()->Subscribe(
                Smp::Services::IEventManager::SMP_PostSimTimeChangeId,
                epf.get());
        simStart();
        waitSimEnd();

        CPPUNIT_ASSERT_EQUAL(4,(int)scheduledTime.size());
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[0]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[1]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[2]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[3]);
    }

    void testScheduleLongTask() {
        auto cb = Callback::create([]() { ::usleep(100000UL); });  // 100 ms sleep
        auto ep = std::make_unique<EntryPoint>(std::move(cb), "callback");

        _scheduler->AddSimulationTimeEvent(ep.get(), 10);
        _scheduler->AddSimulationTimeEvent(ep.get(), 10);
        _scheduler->AddSimulationTimeEvent(ep.get(), 10);
        _scheduler->AddSimulationTimeEvent(ep.get(), 10);
        _scheduler->AddSimulationTimeEvent(ep.get(), 10);

        auto runTo = Callback::create([this]() { _scheduler->step(); });
        ChronoTool::Record rec = ChronoTool::execution(*runTo);

        auto duration_ms = rec.count<std::chrono::milliseconds>();
        TRACE("recorded duration = " << duration_ms << " ms")
        CPPUNIT_ASSERT(duration_ms - 500 < 20);  // 20ms margin for an expected 500ms execution time
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestScheduler);
}  // namespace test
