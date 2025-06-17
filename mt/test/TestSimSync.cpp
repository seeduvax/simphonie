/*
 * @file TestSimSync.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <chrono>
#include "Smp/ISimpleField.h"
#include "Smp/ISimulator.h"
#include "Smp/Services/IEventManager.h"
#include "Smp/Services/IScheduler.h"
#include "abs/test.h"
#include "simdeck/EPPModel.hpp"
#include "simphonie/colibry/SimControl.hpp"
#include "simphonie/colibry/Synchronizer.hpp"
#include "simphonie/kern/Simulator.hpp"
#include "simphonie/mt/SimSyncMaster.hpp"
#include "simphonie/mt/SimSyncSlave.hpp"

namespace test {
using namespace simphonie::mt;

ABS_TEST_SUITE_BEGIN(SimSync)
ABS_TEST_DESCR(test the master / slave synchronization)

private:
class Model : public simdeck::EPPModel {
public:
    inline Model(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent, uint32_t sleep)
        : EPPModel(name, descr, parent),
          _canIRecv(true),
          _recv(true),
          errors(0),
          _sleep(std::chrono::nanoseconds(sleep)) {
        addEP("step", "", this, &Model::step);
    }
    inline void step() {
        _canIRecv = false;
        logInfo("call");
        if (!_recv) {
            logError("Transmitted while the other side was not able to do so");
            errors++;
            _recv = false;
        }
        std::this_thread::sleep_for(_sleep);
        _canIRecv = true;
    }

    Smp::UInt32 errors;

private:
    inline void publish(Smp::IPublication* _receiver) {
        _receiver->PublishField("errors", "", &errors, Smp::ViewKind::VK_All, false, false, true);
        _receiver->PublishField("canIRecv", "", &_canIRecv, Smp::ViewKind::VK_All, false, true, false);
        _receiver->PublishField("recv", "", &_recv, Smp::ViewKind::VK_All, false, false, true);
    }
    inline void connect() {
        getSimulator()->GetScheduler()->AddSimulationTimeEvent(GetEntryPoint("step"), 0, 100000000, -1);
    }

    Smp::Bool _recv, _canIRecv;
    const std::chrono::nanoseconds _sleep;
};

class WaitEndEp : public simdeck::EntryPoint {
public:
    WaitEndEp(Smp::IObject* parent) : EntryPoint("waitEnd", "", parent) {}
    void Execute() const override {
        WaitEndEp* th = (WaitEndEp*)this;
        {
            Synchronized(th->_mutex);
            th->_completed = true;
        }
        th->_monitor.notify_all();
    }
    void wait() {
        Synchronized(_mutex);
        while (!_completed) {
            MonitorWait(_monitor);
        }
    }
    bool _completed = false;
    std::mutex _mutex;
    std::condition_variable _monitor;
};

Smp::ISimulator *_sim, *_sim2;
SimSyncMaster* _master;
SimSyncSlave* _slave;
simphonie::colibry::SimControl* _ctrl;
simphonie::colibry::Synchronizer* _sync;
Model *_mdl, *_mdl2;

public:
void setUp() {
    _sim = new simphonie::kern::Simulator("sim", "", nullptr);
    _sim2 = new simphonie::kern::Simulator("sim2", "", nullptr);
    _master = new SimSyncMaster("master", "", _sim);
    _slave = new SimSyncSlave("slave", "", _sim2);
    _ctrl = new simphonie::colibry::SimControl("ctrl", "", _sim);

    _sim->AddService(_master);
    _sim2->AddService(_slave);
    _sim->AddService(_ctrl);
}

void runAndTest() {
    _sim->AddModel(_mdl);
    _sim2->AddModel(_mdl2);
    _sim->Publish();
    _sim2->Publish();
    dynamic_cast<Smp::ISimpleField*>(_master->GetField("slave"))
        ->SetValue(Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt64, reinterpret_cast<Smp::UInt64>(_slave)));
    dynamic_cast<Smp::IOutputField*>(_master->GetField("inputs"))->Connect(_mdl->GetField("recv"));
    dynamic_cast<Smp::IOutputField*>(_master->GetField("outputs"))->Connect(_mdl->GetField("canIRecv"));
    dynamic_cast<Smp::IOutputField*>(_slave->GetField("inputs"))->Connect(_mdl2->GetField("recv"));
    dynamic_cast<Smp::IOutputField*>(_slave->GetField("outputs"))->Connect(_mdl2->GetField("canIRecv"));
    _sim->Configure();
    _sim->Connect();

    _sim->GetScheduler()->AddSimulationTimeEvent(_master->GetEntryPoint("sync"), 0, 500000000, -1);
    _sim2->GetScheduler()->AddSimulationTimeEvent(_slave->GetEntryPoint("sync"), 300000000, 500000000, -1);
    _ctrl->setCondition("(>= /TimeKeeper/simTime 1000000000)");
    _ctrl->applyCondition();
    {
        WaitEndEp wep(_sim);
        _sim->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_LeaveExecutingId, &wep);
        _sim->Run();
        wep.wait();
    }

    CPPUNIT_ASSERT(0 == _mdl->errors);
    CPPUNIT_ASSERT(0 == _mdl2->errors);
}

void tearDown() {
    delete _sim2;
    delete _sim;
}

ABS_TEST_CASE_BEGIN(MasterWaitingForSlave) {
    _mdl = new Model("mdl", "", _sim, 0);
    _mdl2 = new Model("mdl2", "", _sim2, 10000000);
    runAndTest();
}
ABS_TEST_CASE_END

ABS_TEST_CASE_BEGIN(SlaveWaitingForMaster) {
    _mdl = new Model("mdl", "", _sim, 10000000);
    _mdl2 = new Model("mdl2", "", _sim2, 0);
    runAndTest();
}
ABS_TEST_CASE_END

ABS_TEST_SUITE_END
} /* namespace test */
