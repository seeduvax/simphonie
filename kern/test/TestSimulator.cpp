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
#include "simph/kern/Simulator.hpp"
#include "simph/kern/AStepMdl.hpp"
#include "simph/sys/Logger.hpp"
#include "simph/smpdk/Collection.hpp"
#include "simph/smpdk/Container.hpp"
#include "simph/smpdk/EntryPoint.hpp"
#include "simph/sys/Synchro.hpp"


namespace test {
using namespace simph::kern;

class CompositeModel: public AStepMdl, virtual public Smp::IComposite {
public:
    CompositeModel(Smp::String8 name, Smp::String8 descr, Smp::IComposite* parent): 
            AStepMdl(name,descr,parent), _containers("Containers", "", this) {
    }
    void step() override {
        TRACE(""<<GetName()<<".step()");
    }
    const Smp::ContainerCollection* GetContainers() const override {
        return &_containers;
    }
    Smp::IContainer* GetContainer(Smp::String8 name) const override {
        return _containers.at(name);
    }
protected:
    // on configure, add a submodel when the parent is the simulator only (to
    // avoid infinte recursion).
    // Used in test to check lately created components are well handled by
    // the simulator.
    void configure() override {
        if (GetParent()==getSimulator()) {
            auto c=new simph::smpdk::Container("sub","",this);
            _containers.push_back(c);
            c->AddComponent(new CompositeModel("childMdl","",this));
        }
    }
private:
    simph::smpdk::CollectionOwner<Smp::IContainer> _containers;
};


// ----------------------------------------------------------
// test fixture implementation
class TestSimulator : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestSimulator);
    CPPUNIT_TEST(testStates);
    CPPUNIT_TEST_SUITE_END();

private:
    Smp::ISimulator* _sim=nullptr;
    bool _completed=false;
    Smp::Duration _endSimTime=1e9;
    std::mutex _mutex;
    std::condition_variable _monitor;
    Smp::IEntryPoint* _checkEndSim;
    Smp::IEntryPoint* _notifyEndSim;
    bool _endReached=false;

public:
    void setUp() {
        _checkEndSim=new simph::smpdk::EntryPoint(this,&TestSimulator::checkEndSim,"checkEndSim");
        _notifyEndSim=new simph::smpdk::EntryPoint(this,&TestSimulator::notifyEndSim,"notifyEndSim");
    }

    void tearDown() {
        delete _checkEndSim;
        delete _notifyEndSim;
    }

    void checkEndSim() {
        if (_sim->GetTimeKeeper()->GetSimulationTime()>=_endSimTime) {
            if (!_endReached) {
               _sim->Hold(true);
            }
            _endReached=true;
        }
    }

    void notifyEndSim() {
        {
            Synchronized(_mutex);
            _completed=true;
        }
        _monitor.notify_all();
    }


    void testStates() {
        _sim=new Simulator();
        _sim->AddModel(new CompositeModel("parentMdl","",_sim));
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
