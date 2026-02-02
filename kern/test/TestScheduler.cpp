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
#include "simdeck/front/SimFront.hpp"

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
            if (_vv!=nullptr) {
                auto st = _ts._sim->GetTimeKeeper()->GetSimulationTime();
                _vv->push_back(st);
                TRACE("" << st);
                if (_vv->size()>=4) {
                    // emulate simulator stop after 4 iterations
                    _ts._sim->Hold(true);
                }
            }
            _sequence.append(1,'R');
        }
        void epDelay() {
            ::usleep(100000UL);
            _sequence.append(1,'D');
        }
        void ep1() {
            _sequence.append(1,'1');
        }
        void ep2() {
            _sequence.append(1,'2');
        }
        void ep3() {
            _sequence.append(1,'3');
        }
        void epAddImmediate() {
            if (_immediateEp!=nullptr) {
                _ts._scheduler->AddImmediateEvent(_immediateEp);        
            }
            _sequence.append(1,'I');
        }
        TestScheduler& _ts;
        std::vector<Smp::Duration>* _vv=nullptr;
        std::string _sequence="";
        Smp::IEntryPoint* _immediateEp=nullptr;
        Scheduler* _scheduler;
    };
    Simulator* _sim;
    Scheduler* _scheduler;
    std::mutex _mutex;
    std::condition_variable _monitor;
    Smp::IEntryPoint* _epLeaveExecuting;
    bool _run=false;
    EPSet* _epset;
    Smp::Duration _endSimTime = 3600000000000ULL; 

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

    ABS_TEST_CASE_BEGIN(Schedule) {
        std::vector<Smp::Duration> scheduledTime;
        _epset->_vv=&scheduledTime;
        auto ep = EntryPoint::Create("cb","",_epset, &EPSet::epRecSimTime);

        _scheduler->AddSimulationTimeEvent(ep, 10);
        _scheduler->AddSimulationTimeEvent(ep, 30);
        _scheduler->AddSimulationTimeEvent(ep, 20);
        _scheduler->AddSimulationTimeEvent(ep, 20);

        simdeck::front::SimFront front(_sim);
        front.SetEndSimulationTime(_endSimTime);
        front.Run();
        // wait simulation end with 10sec zulu timeout.
        CPPUNIT_ASSERT(front.Wait(Smp::SimulatorStateKind::SSK_Standby, 10000000000L));
        delete ep;
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[0]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)20, scheduledTime[1]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)20, scheduledTime[2]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)30, scheduledTime[3]);
        std::string expSeq="RRRR";
        CPPUNIT_ASSERT_EQUAL(expSeq,_epset->_sequence);
    }
    ABS_TEST_CASE_END

    ABS_TEST_CASE_BEGIN(Schedule2) {
        std::vector<Smp::Duration> scheduledTime;
        _epset->_vv=&scheduledTime;
        auto epcb = EntryPoint::Create("cb","",_epset, &EPSet::epRecSimTime);

        _scheduler->AddSimulationTimeEvent(epcb, 10);
        _scheduler->AddSimulationTimeEvent(epcb, 10);
        _scheduler->AddSimulationTimeEvent(epcb, 10);
        _scheduler->AddSimulationTimeEvent(epcb, 10);


        simdeck::front::SimFront front(_sim); 
        front.Run();
        CPPUNIT_ASSERT(front.Wait(Smp::SimulatorStateKind::SSK_Standby, 10000000000L));
        delete epcb;

        CPPUNIT_ASSERT_EQUAL(4,(int)scheduledTime.size());
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[0]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[1]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[2]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10, scheduledTime[3]);
        std::string expSeq="RRRR";
        CPPUNIT_ASSERT_EQUAL(expSeq,_epset->_sequence);
    }
    ABS_TEST_CASE_END

    ABS_TEST_CASE_BEGIN(ScheduleLongTask) {
        auto ep = EntryPoint::Create("cb","",_epset, &EPSet::epDelay);

        _scheduler->AddSimulationTimeEvent(ep, 10);
        _scheduler->AddSimulationTimeEvent(ep, 10);
        _scheduler->AddSimulationTimeEvent(ep, 10);
        _scheduler->AddSimulationTimeEvent(ep, 10);
        _scheduler->AddSimulationTimeEvent(ep, 10);

        simdeck::front::SimFront front(_sim); 
        front.SetEndSimulationTime(_endSimTime);
        const auto start = std::chrono::steady_clock::now();
        front.Run();
        CPPUNIT_ASSERT(front.Wait(Smp::SimulatorStateKind::SSK_Standby, 10000000000L));
        const auto end = std::chrono::steady_clock::now();

        auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        delete ep;
        TRACE("recorded duration = " << duration_ms << " ms")
// TODO this assert is bullshit: when too fast, assert is true.
        CPPUNIT_ASSERT(duration_ms - 500 < 20);  // 20ms margin for an expected 500ms execution time
        std::string expSeq="DDDDD";
        CPPUNIT_ASSERT_EQUAL(expSeq,_epset->_sequence);
    }
    ABS_TEST_CASE_END

    ABS_TEST_CASE_BEGIN(SequenceOrder) {
        auto epRec = EntryPoint::Create("rec","",_epset, &EPSet::epRecSimTime);
        auto ep1 = EntryPoint::Create("e1","",_epset, &EPSet::ep1);
        auto ep2 = EntryPoint::Create("e2","",_epset, &EPSet::ep2);
        auto ep3 = EntryPoint::Create("e3","",_epset, &EPSet::ep3);
        auto epOnce = EntryPoint::Create("add","",_epset, &EPSet::epAddImmediate);
        auto epDelay = EntryPoint::Create("delay","",_epset, &EPSet::epDelay);
        _epset->_immediateEp=epRec;
        // define a schedule with periodic tasks and finite repeat and
        // some immediate event that shall insert at the right place. 
        _scheduler->AddSimulationTimeEvent(ep1, 0,10,10);
        _scheduler->AddSimulationTimeEvent(epOnce, 40);
        _scheduler->AddSimulationTimeEvent(ep2, 0,20,10);
        _scheduler->AddSimulationTimeEvent(ep3, 0,10,10);
        _scheduler->AddSimulationTimeEvent(epDelay, -1,10,10); // shall never run.
        simdeck::front::SimFront front(_sim); 
        front.SetEndSimulationTime(_endSimTime);
        const auto start = std::chrono::steady_clock::now();
        front.Run();
        CPPUNIT_ASSERT(front.Wait(Smp::SimulatorStateKind::SSK_Standby, 10000000000L));
        //                  0  1 2  3 4    5 6  7 8  9 A
        std::string expSeq="12313123131IR2313123131231312322222";
        CPPUNIT_ASSERT_EQUAL(expSeq,_epset->_sequence);
        delete epRec;
        delete ep1;
        delete ep2;
        delete ep3;
        delete epOnce;
        delete epDelay;
    }
    ABS_TEST_CASE_END

ABS_TEST_SUITE_END
}  // namespace test
