/*
 * @file TestSimulator.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "abs/test.h"
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
#include "simdeck/front/SimFront.hpp"


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
            _innerModel = new CModel("childMdl", "", this);
            GetContainer("sub")->AddComponent(_innerModel);
        }
    }

private:
    CModel* _innerModel = nullptr;
};


// ----------------------------------------------------------
// test fixture implementation
ABS_TEST_SUITE_BEGIN(Simulator)

private:
    Smp::ISimulator* _sim=nullptr;
    bool _completed=false;
    Smp::Duration _endSimTime=1e9;
    std::mutex _mutex;
    std::condition_variable _monitor;
    Smp::IEntryPoint* _checkEndSim;
    Smp::IEntryPoint* _notifyEndSim;
    bool _endReached=false;

    typedef simdeck::TEntryPoint<TestSimulator*, void (TestSimulator::*)()> EP;

public:
    void setUp() {
        _sim=new Simulator();
    }

    void tearDown() {
        delete _sim;
    }

    ABS_TEST_CASE_BEGIN(States) {
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
        simdeck::front::SimFront front(_sim);
        front.SetEndSimulationTime(_endSimTime);
        front.Run();
        CPPUNIT_ASSERT(front.Wait(Smp::SimulatorStateKind::SSK_Standby, 10000000000L));
        CPPUNIT_ASSERT(_sim->GetTimeKeeper()->GetSimulationTime()>=_endSimTime);
        CPPUNIT_ASSERT_EQUAL(Smp::SimulatorStateKind::SSK_Standby, _sim->GetState());
    }
    ABS_TEST_CASE_END

ABS_TEST_SUITE_END
}  // namespace test
