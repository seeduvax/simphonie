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
#include "abs/test.h"
#include "simdeck/EPPModel.hpp"
#include "simphonie/colibry/SimControl.hpp"
#include "simphonie/colibry/Synchronizer.hpp"
#include "simphonie/kern/Simulator.hpp"

namespace test {
using namespace simphonie::colibry;

// ----------------------------------------------------------
// test suite implementation
ABS_TEST_SUITE_BEGIN(Synchronizer)

private:
class _UselessModel : public simdeck::EPPModel {
public:
    inline _UselessModel(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
        : EPPModel(name, descr, parent), _sleep(30000000lu) {
        addEP("step", "", this, &_UselessModel::step);
    }
    inline void step() {
        logInfo("call");
        std::this_thread::sleep_for(_sleep);
        _sleep *= 2lu;
    }

private:
    inline void connect() {
        getSimulator()->GetScheduler()->AddSimulationTimeEvent(GetEntryPoint("step"), 0l, 100000000l, -1);
    }

    std::chrono::nanoseconds _sleep;
};

simphonie::kern::Simulator* _sim;
Synchronizer* _sync;
SimControl* _ctrl;
_UselessModel* _model;

public:
void setUp() {
    _sim = new simphonie::kern::Simulator("TestSynchronizationSimu", "", nullptr);
    _sync = new Synchronizer("TestSynchronizationSync", "", _sim);
    _ctrl = new SimControl("TestSynchronizationSimCtrl", "", _sim);
    _model = new _UselessModel("TestSynchronizationMdl", "", _sim);

    _sim->Initialise();
    _sim->AddService(_sync);
    _sim->AddService(_ctrl);
    _sim->AddModel(_model);
    _sim->Publish();
    _sim->Configure();
    {
        const Smp::Duration period = 100000000lu;
        dynamic_cast<Smp::ISimpleField*>(_sim->GetResolver()->ResolveRelative("period", _sync))
            ->SetValue(Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int64, period));
    }
    _sim->Connect();
}

void tearDown() {
    delete _sim;
}

ABS_TEST_CASE_BEGIN(Synchronizer) {
    ABS_TEST_CASE_REQ(simph.sync .1)
    ABS_TEST_CASE_REQ(simph.sync .2)
    ABS_TEST_CASE_REQ(simph.sync .3)
    ABS_TEST_CASE_REQ(simph.sync .4)
    ABS_TEST_CASE_REQ(simph.sync .5)

    _ctrl->setCondition("(>= /TimeKeeper/simTime 300000000)");
    _ctrl->applyCondition();
    _sim->Run();
    while (_sim->GetState() == Smp::SimulatorStateKind::SSK_Executing) {}

    const auto overflowCount =
        dynamic_cast<Smp::ISimpleField*>(_sim->GetResolver()->ResolveRelative("overflowCounter", _sync))
            ->GetValue()
            .value.uInt64Value;
    const auto lastMargin = dynamic_cast<Smp::ISimpleField*>(_sim->GetResolver()->ResolveRelative("margin", _sync))
                                ->GetValue()
                                .value.int64Value;

    CPPUNIT_ASSERT(1lu <= overflowCount);
    CPPUNIT_ASSERT(0 >= lastMargin);
}
ABS_TEST_CASE_END

ABS_TEST_SUITE_END
}  // namespace test
