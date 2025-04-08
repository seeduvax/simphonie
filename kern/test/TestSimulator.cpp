/*
 * @file TestSimulator.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simphonie/kern/Simulator.hpp"
#include "simphonie/kern/EventManager.hpp"
#include "simphonie/sys/Logger.hpp"
#include "simdeck/EPPModel.hpp"
#include "simdeck/EntryPoint.hpp"
#include "simdeck/EntryPointPublisher.hpp"
#include "simphonie/sys/Synchro.hpp"
#include "Smp/Services/ITimeKeeper.h"
#include "Smp/Services/IScheduler.h"
#include "Smp/Services/IEventManager.h"


namespace test {
using namespace simphonie::kern;
using namespace simdeck;

class CModel: public simdeck::EPPModel, virtual public simdeck::AComposite {
public:
    CModel(Smp::String8 name, Smp::String8 descr, Smp::IComposite* parent): 
            simdeck::EPPModel(name,descr,parent) {
        addContainer("sub");
        addEP("step", "", this, &CModel::step);
    }
    virtual ~CModel() {
        if (_innerModel != nullptr) {
            GetContainer("sub")->DeleteComponent(_innerModel);
        }
    }

    void step() {
        TRACE(""<<GetName()<<".step()");
    }
protected:
    // on configure, add a submodel when the parent is the simulator only (to
    // avoid infinte recursion.
    // Used in test to check lately created components are well handled by
    // the simulator.
    void configure() override {
        if (GetParent()==getSimulator()) {
            _innerModel = new CModel("childMdl","",this);
            GetContainer("sub")->AddComponent(_innerModel);
        }
    }

private:
    CModel* _innerModel = nullptr;
};


// ----------------------------------------------------------
// test fixture implementation
class TestSimulator : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestSimulator);
    CPPUNIT_TEST(testStates);
    CPPUNIT_TEST_SUITE_END();

private:
    class EPSet: public Object {
    public:
        EPSet(TestSimulator& ts): 
                Object("epset","",nullptr), _ts(ts) 
        {
        } 
        virtual ~EPSet() {}
        void checkEndSim() {
            if (_ts._sim->GetTimeKeeper()->GetSimulationTime()>=_ts._endSimTime) {
                if (!_ts._endReached) {
                   _ts._sim->Hold(true);
                }
                _ts._endReached=true;
            }
        }
        void notifyEndSim() {
            {
                Synchronized(_ts._mutex);
                _ts._completed=true;
            }
            _ts._monitor.notify_all();
        }
        TestSimulator& _ts;
    };

    Smp::ISimulator* _sim=nullptr;
    bool _completed=false;
    Smp::Duration _endSimTime=1e9;
    std::mutex _mutex;
    std::condition_variable _monitor;
    Smp::IEntryPoint* _checkEndSim;
    Smp::IEntryPoint* _notifyEndSim;
    bool _endReached=false;
    EPSet* _epset;

    typedef simdeck::TEntryPoint<TestSimulator*, void (TestSimulator::*)()> EP;

public:
    void setUp() {
        _epset=new EPSet(*this);
        _checkEndSim = EntryPoint::Create("checkEndSim", "", _epset, &EPSet::checkEndSim);
        _notifyEndSim = EntryPoint::Create("notifyEndSim", "", _epset, &EPSet::notifyEndSim);
    }

    void tearDown() {
        delete _checkEndSim;
        delete _notifyEndSim;
        delete _epset;
        delete _sim;
    }

    void testStates() {
        _sim=new Simulator();
        _sim->AddModel(new CModel("parentMdl","",_sim));
        _sim->Publish();
        // after publish, only parent model is expected.
        CPPUNIT_ASSERT(_sim->GetResolver()->ResolveAbsolute("parentMdl")!=nullptr);
        CPPUNIT_ASSERT(_sim->GetResolver()->ResolveAbsolute("parentMdl/childMdl")==nullptr);

        _sim->Configure();
        // after configure, submodel shuld have been created and published as
        // well.
        CPPUNIT_ASSERT(_sim->GetResolver()->ResolveAbsolute("parentMdl/childMdl")!=nullptr);

        _sim->Connect();
        auto ep=dynamic_cast<Smp::IEntryPoint*>(_sim->GetResolver()->ResolveAbsolute("parentMdl/step"));
        CPPUNIT_ASSERT(ep!=nullptr);
        _sim->GetScheduler()->AddSimulationTimeEvent(ep,0,1e8,-1);

        // register to event manager to catch simulation time change.
        _sim->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_PostSimTimeChangeId,_checkEndSim);
        _sim->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterStandbyId,_notifyEndSim);

        CPPUNIT_ASSERT_EQUAL((Smp::Duration)0,_sim->GetTimeKeeper()->GetSimulationTime());
        _completed=false;
        _sim->Run();
        {
            Synchronized(_mutex);
            while(!_completed) {
                MonitorWait(_monitor);
            }
        }
        CPPUNIT_ASSERT(_sim->GetTimeKeeper()->GetSimulationTime()>=_endSimTime);
        CPPUNIT_ASSERT_EQUAL(Smp::SimulatorStateKind::SSK_Standby,_sim->GetState());
        _sim->Exit();
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestSimulator);
}  // namespace test
