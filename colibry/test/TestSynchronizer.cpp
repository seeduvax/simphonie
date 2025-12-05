/*
 * @file TestSynchronizer.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <chrono>
#include <thread>
#include "Smp/ISimpleField.h"
#include "Smp/Services/IResolver.h"
#include "Smp/Services/IScheduler.h"
#include "Smp/Services/IEventManager.h"
#include "abs/test.h"
#include "simdeck/EPPModel.hpp"
#include "simphonie/colibry/SimControl.hpp"
#include "simphonie/colibry/Synchronizer.hpp"
#include "simphonie/kern/Simulator.hpp"
#include "simphonie/sys/Synchro.hpp"

namespace test {
using namespace simphonie::colibry;

// ----------------------------------------------------------
// test suite implementation
ABS_TEST_SUITE_BEGIN(Synchronizer)

private:
class UselessModel : public simdeck::EPPModel {
public:
    inline UselessModel(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
        : EPPModel(name, descr, parent), _sleep(10000000lu) {
        addEP("step", "", this, &UselessModel::step);
    }
    inline void step() {
        logInfo("call");
        if (_cpt==0) {
            // try to set thread realtime with max priority
            struct sched_param sp;
            int schedPolicy;
            pthread_getschedparam(pthread_self(), &schedPolicy, &sp);
            schedPolicy=SCHED_RR;
            sp.sched_priority=sched_get_priority_max(schedPolicy);
            int cr=pthread_setschedparam(pthread_self(), schedPolicy, &sp);
            if (cr!=0) {
                std::cerr << "Can't set thread schedule parameters: " << strerror(errno) << std::endl;
            }
        }
        std::this_thread::sleep_for(_sleep);
        if (_overflowFlag && (_cpt % 10) == 3) {
            std::this_thread::sleep_for(_sleep*10);
        }
        _cpt++;
    }

    bool _overflowFlag = false;
private:
    inline void connect() {
        getSimulator()->GetScheduler()->AddSimulationTimeEvent(GetEntryPoint("step"), 0l, 100000000l, -1);
    }

    std::chrono::nanoseconds _sleep;
    int _cpt=0;
};

simphonie::kern::Simulator* _sim;
Synchronizer* _sync;
SimControl* _ctrl;
UselessModel* _model;

class WaitEndEp: public simdeck::EntryPoint {
public:
    WaitEndEp(Smp::IObject* parent): EntryPoint("waitEnd", "", parent) {
    }
    void Execute() const override {
        WaitEndEp *th=(WaitEndEp*)this;
        {
            Synchronized(th->_mutex);
            th->_completed=true;
        }
        th->_monitor.notify_all();
    }
    void wait() {
        Synchronized(_mutex);
        while (!_completed) {
            MonitorWait(_monitor);
        }
    }
    bool _completed=false;
    std::mutex _mutex;
    std::condition_variable _monitor;
};

public:
void setUp() {
    _sim = new simphonie::kern::Simulator("TestSynchronizationSimu", "", nullptr);
    _sync = new Synchronizer("TestSynchronizationSync", "", _sim);
    _ctrl = new SimControl("TestSynchronizationSimCtrl", "", _sim);
    _model = new UselessModel("TestSynchronizationMdl", "", _sim);

    _sim->Initialise();
    _sim->AddService(_sync);
    _sim->AddService(_ctrl);
    _sim->AddModel(_model);
    _sim->Publish();
    _sim->Configure();
    {
        const Smp::Duration period = 100000000lu;
        dynamic_cast<Smp::ISimpleField*>(_sync->GetField("period"))
            ->SetValue(Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int64, period));
    }
    _sim->Connect();
}

void tearDown() {
    delete _sim;
}

void runSim() {
    _ctrl->setCondition("(>= /TimeKeeper/simTime 3000000000)");
    _ctrl->applyCondition();
    WaitEndEp wep(_sim);
    _sim->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_LeaveExecutingId,&wep);
    _sim->Run();
    wep.wait();

}

ABS_TEST_CASE_BEGIN(NoOverFlow) {
    ABS_TEST_CASE_REQ(simph.sync.1)
    ABS_TEST_CASE_REQ(simph.sync.2)
    ABS_TEST_CASE_REQ(simph.sync.3)
    ABS_TEST_CASE_REQ(simph.sync.4)
    ABS_TEST_CASE_REQ(simph.sync.5)
    runSim();
    const auto overflowCount =
        dynamic_cast<Smp::ISimpleField*>(_sim->GetResolver()->ResolveRelative("overflowCounter", _sync))
            ->GetValue()
            .value.uInt64Value;
    CPPUNIT_ASSERT_EQUAL((uint64_t)0,overflowCount);
}
ABS_TEST_CASE_END

ABS_TEST_CASE_BEGIN(OverFlow) {
    _model->_overflowFlag = true;
    runSim();
    const auto overflowCount =
        dynamic_cast<Smp::ISimpleField*>(_sim->GetResolver()->ResolveRelative("overflowCounter", _sync))
            ->GetValue()
            .value.uInt64Value;
    CPPUNIT_ASSERT_EQUAL((uint64_t)3,overflowCount);
}
ABS_TEST_CASE_END

ABS_TEST_SUITE_END
}  // namespace test
