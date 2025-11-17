/*
 * @file TestMetaScheduler.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "abs/test.h"
#include "simphonie/colibry/MetaScheduler.hpp"
#include "simphonie/kern/Simulator.hpp"
#include "simdeck/EPPModel.hpp"
#include "simphonie/sys/Synchro.hpp"
 
 namespace test {
 using namespace simphonie::colibry;
 using namespace simphonie::kern;
 using namespace simphonie::sys;

#define EVENT1 "TestEvent1"
#define EVENT2 "TestEvent2"


 
 // ----------------------------------------------------------
 // test suite implementation
 ABS_TEST_SUITE_BEGIN( MetaScheduler )
 // uncomment and complete next line for test suite description
 // ABS_TEST_DESCR(test description)
 
 private:
    // A class defining few dummy methods to be used as entry points.
    class TModel: public simdeck::EPPModel {
    public:
        TModel(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent): 
                simdeck::EPPModel(name, descr, parent) {
            addEP("date","",this, &TModel::epDate);
            addEP("cpt1","",this, &TModel::epIncCpt1);
            addEP("cpt2","",this, &TModel::epIncCpt2);
            addEP("ev1","",this, &TModel::epEmitEv1);
            addEP("ev2","",this, &TModel::epEmitEv2);
            addEP("stopSim","",this, &TModel::epHoldOnTimeElapsed);
            addEP("leaveExec","",this, &TModel::epLeaveExecuting);
        }
        virtual ~TModel() {
        }

        void epDate() {
            _dates.push_back(getSimulator()->GetTimeKeeper()->GetSimulationTime());
        }
        void epIncCpt1() {
            _lastDate1=getSimulator()->GetTimeKeeper()->GetSimulationTime();
            if (_firstDate1<0) {
                _firstDate1=_lastDate1;
            }
            _cpt1++;
        }
        void epIncCpt2() {
            _lastDate2=getSimulator()->GetTimeKeeper()->GetSimulationTime();
            if (_firstDate2<0) {
                _firstDate2=_lastDate2;
            }
            _cpt2++;
        }
        void epEmitEv1() {
            auto evMgr=getSimulator()->GetEventManager();
            evMgr->Emit(evMgr->QueryEventId(EVENT1));
        }
        void epEmitEv2() {
            auto evMgr=getSimulator()->GetEventManager();
            evMgr->Emit(evMgr->QueryEventId(EVENT2));
        }

        void epHoldOnTimeElapsed() {
            auto sim=getSimulator();
            if (sim->GetTimeKeeper()->GetSimulationTime()>=_endSimTime) {
                sim->Hold(true);
            }
        }
        void epLeaveExecuting() {
            {
                Synchronized(_mutex);
                _run=false;
            }
            MonitorNotifyAll(_monitor);
        }
        void waitLeaveExecuting() {
            Synchronized(_mutex);
            while (_run) {
                MonitorWait(_monitor);
            }
        }

        int _cpt1=0;
        int _cpt2=0;
        Smp::Duration _firstDate1=-1;
        Smp::Duration _firstDate2=-1;
        Smp::Duration _lastDate1=-1;
        Smp::Duration _lastDate2=-1;
        std::vector<Smp::Duration> _dates;
        bool _run=true;
        std::mutex _mutex;
        std::condition_variable _monitor;
        Smp::Duration _endSimTime=INT64_MAX;
    };


    Smp::ISimulator* _sim=nullptr;
    MetaScheduler* _metaScheduler=nullptr;
    TModel* _mdl=nullptr;
 
public:
    void setUp() {
        _sim=new Simulator("tsim","test simulator",nullptr);
        _metaScheduler=new MetaScheduler("msched","test meta scheduler", _sim);
        _sim->AddService(_metaScheduler);
        _mdl=new TModel("model", "test model", _sim);
        _sim->AddModel(_mdl);
        _sim->Publish();
        _sim->Configure();
        _sim->Connect();
        _sim->Initialise();
        auto evMgr=_sim->GetEventManager();
        evMgr->Subscribe(Smp::Services::IEventManager::SMP_PostSimTimeChangeId, _mdl->GetEntryPoint("stopSim"));
        evMgr->Subscribe(Smp::Services::IEventManager::SMP_LeaveExecutingId, _mdl->GetEntryPoint("leaveExec"));
    }
     
    void tearDown() {
        delete _sim;
    }
 
 
    ABS_TEST_CASE_BEGIN(OnEventACyclicSched) {
        ABS_TEST_DESCR(Schedule one ep, to run once each time one event is emitted)
        auto scheduler=_sim->GetScheduler();
        // schedule event emit at few arbitrary dates
        scheduler->AddSimulationTimeEvent(_mdl->GetEntryPoint("ev1"),100); 
        scheduler->AddSimulationTimeEvent(_mdl->GetEntryPoint("ev1"),1017); 
        scheduler->AddSimulationTimeEvent(_mdl->GetEntryPoint("ev1"),2017); 
        scheduler->AddSimulationTimeEvent(_mdl->GetEntryPoint("ev1"),2038); 
        scheduler->AddSimulationTimeEvent(_mdl->GetEntryPoint("ev1"),3001); 
        // schedule one ep periodically and on event
        auto evMgr=_sim->GetEventManager();
        auto s=_metaScheduler->newSchedule(_mdl->GetEntryPoint("date"));
        s->setSimulationTime(0)
            .setCycleTime(_metaScheduler->getMaxSimTime())
            .setRepeat(-1)
            .setActive(false)
            .subscribeActivateEvent(evMgr->QueryEventId(EVENT1))
            .submit();
        // go sim
        _sim->Run();
        _mdl->waitLeaveExecuting();
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)100,_mdl->_dates[0]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)1017,_mdl->_dates[1]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)2017,_mdl->_dates[2]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)2038,_mdl->_dates[3]);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)3001,_mdl->_dates[4]);
    }
    ABS_TEST_CASE_END

    ABS_TEST_CASE_BEGIN(OnEventCyclicSched) {
        ABS_TEST_DESCR(Schedule 2 ep, one periodically, the other periodicall and one on event)
        ABS_TEST_DESCR(Send events at fixed date, check results with activation dates and counters values)
        ABS_TEST_CASE_REQ(req.id)
        // schedule events emitting at fixed date
        auto scheduler=_sim->GetScheduler();
        scheduler->AddSimulationTimeEvent(_mdl->GetEntryPoint("ev1"),100); 
        scheduler->AddSimulationTimeEvent(_mdl->GetEntryPoint("ev2"),200); 

        // schedule one ep periodically
        auto s1=_metaScheduler->newSchedule(_mdl->GetEntryPoint("cpt1"));
        s1->setSimulationTime(0)
            .setCycleTime(10)
            .setRepeat(-1)
            .submit();

        // schedule one ep periodically and on event
        auto evMgr=_sim->GetEventManager();
        auto s2=_metaScheduler->newSchedule(_mdl->GetEntryPoint("cpt2"));
        s2->setSimulationTime(0)
            .setCycleTime(10)
            .setRepeat(-1)
            .setActive(false)
            .subscribeActivateEvent(evMgr->QueryEventId(EVENT1))
            .subscribeDeactivateEvent(evMgr->QueryEventId(EVENT2))
            .submit();

        // define end simulation time
        _mdl->_endSimTime=300;
     
        // go sim
        _sim->Run();
        _mdl->waitLeaveExecuting();
     
        // check sim results
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)0,_mdl->_firstDate1);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)290,_mdl->_lastDate1);
        CPPUNIT_ASSERT_EQUAL(30,_mdl->_cpt1);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)100,_mdl->_firstDate2);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)190,_mdl->_lastDate2);
        CPPUNIT_ASSERT_EQUAL(10,_mdl->_cpt2);
    }
    ABS_TEST_CASE_END
ABS_TEST_SUITE_END
} // namespace test
