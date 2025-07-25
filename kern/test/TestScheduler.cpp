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

#include "abs/test.h"
#include "simphonie/kern/Scheduler.hpp"
#include "simphonie/kern/Simulator.hpp"
#include "simphonie/sys/Callback.hpp"
#include "simphonie/sys/Logger.hpp"

namespace test {
using namespace simphonie::kern;
using namespace simphonie::sys;

// ----------------------------------------------------------
// test fixture implementation
ABS_TEST_SUITE_BEGIN(Scheduler)

private:
    class EPSet: public Object {
    public:
        EPSet(TestScheduler& ts): 
                Object("epset","",nullptr), _ts(ts) 
        {
        } 
        virtual ~EPSet() {}
        void epLeaveExecuting() {
            {
                Synchronized(_ts._mutex);
                _ts._run=false;
            }
            MonitorNotifyAll(_ts._monitor);
        }
        void epRecSimTime() {
            auto st = _ts._sim->GetTimeKeeper()->GetSimulationTime();
            _vv->push_back(st);
            TRACE("" << st);
            if (_vv->size()>=4) {
                // emulate simulator stop after 4 iterations
                _ts._sim->Hold(true);
            }
        }
        void epDelay() {
            ::usleep(100000UL);
        }
        void epEndSimuCtrl() {
            if (_ts._sim->GetTimeKeeper()->GetSimulationTime()>_endSimTime) {
                TRACE("Requesting simulation end");
                _ts._sim->Hold(true);
            }
        }
        TestScheduler& _ts;
        std::vector<Smp::Duration>* _vv;
        Smp::Duration _endSimTime = 3600000000000ULL; 
    };
    Simulator* _sim;
    Scheduler* _scheduler;
    std::mutex _mutex;
    std::condition_variable _monitor;
    Smp::IEntryPoint* _epLeaveExecuting;
    bool _run=false;
    EPSet* _epset;

public:
    void setUp() {
        _sim = new Simulator();
        _sim->Publish();
        _sim->Configure();
        _sim->Connect();
        _scheduler = dynamic_cast<Scheduler*>(_sim->GetScheduler());
        _epset=new EPSet(*this);
        _epLeaveExecuting=EntryPoint::Create("leaveExecution","",_epset,
                &EPSet::epLeaveExecuting);
        _sim->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_LeaveExecutingId,_epLeaveExecuting);
    }

    void tearDown() {
        delete _epLeaveExecuting;
        delete _epset;
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

    ABS_TEST_CASE_BEGIN(TestSchedule) {
        std::vector<Smp::Duration> scheduledTime;
        _epset->_vv=&scheduledTime;
        auto ep = EntryPoint::Create("cb","",_epset, &EPSet::epRecSimTime);

        _scheduler->AddSimulationTimeEvent(ep, 10);
        _scheduler->AddSimulationTimeEvent(ep, 30);
        _scheduler->AddSimulationTimeEvent(ep, 20);
        _scheduler->AddSimulationTimeEvent(ep, 20);
        simStart();
        waitSimEnd();
        delete ep;
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[0]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)20, scheduledTime[1]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)20, scheduledTime[2]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)30, scheduledTime[3]);
    }
    ABS_TEST_CASE_END

    ABS_TEST_CASE_BEGIN(TestSchedule2) {
        std::vector<Smp::Duration> scheduledTime;
        _epset->_vv=&scheduledTime;
        auto epcb = EntryPoint::Create("cb","",_epset, &EPSet::epRecSimTime);

        _scheduler->AddSimulationTimeEvent(epcb, 10);
        _scheduler->AddSimulationTimeEvent(epcb, 10);
        _scheduler->AddSimulationTimeEvent(epcb, 10);
        _scheduler->AddSimulationTimeEvent(epcb, 10);
auto ev=
        _scheduler->AddSimulationTimeEvent(epcb, 20);
_scheduler->SetEventStartOnEvent(ev, 3);



        _epset->_endSimTime=10;
        auto epf=EntryPoint::Create("end","",_epset,
                &EPSet::epEndSimuCtrl);
        _sim->GetEventManager()->Subscribe(
                Smp::Services::IEventManager::SMP_PostSimTimeChangeId,
                epf);
        simStart();
        waitSimEnd();
        delete epcb;
        delete epf;

        CPPUNIT_ASSERT_EQUAL(4,(int)scheduledTime.size());
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[0]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[1]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[2]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[3]);
    }
    ABS_TEST_CASE_END

    ABS_TEST_CASE_BEGIN(TestScheduleLongTask) {
        auto ep = EntryPoint::Create("cb","",_epset, &EPSet::epDelay);

        _scheduler->AddSimulationTimeEvent(ep, 10);
        _scheduler->AddSimulationTimeEvent(ep, 10);
        _scheduler->AddSimulationTimeEvent(ep, 10);
        _scheduler->AddSimulationTimeEvent(ep, 10);
        _scheduler->AddSimulationTimeEvent(ep, 10);

        const auto start = std::chrono::steady_clock::now();
        _scheduler->step();
        const auto end = std::chrono::steady_clock::now();

        auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        delete ep;
        TRACE("recorded duration = " << duration_ms << " ms")
        CPPUNIT_ASSERT(duration_ms - 500 < 20);  // 20ms margin for an expected 500ms execution time
    }
    ABS_TEST_CASE_END

    ABS_TEST_SUITE_END
}  // namespace test
