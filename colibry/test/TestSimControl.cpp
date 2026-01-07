/*
 * @file TestSimControl.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <exception>
#include "Smp/IPublication.h"
#include "abs/test.h"
#include "simdeck/EPPModel.hpp"
#include "simdeck/StringField.hpp"
#include "simphonie/colibry/SimControl.hpp"
#include "simphonie/kern/EventManager.hpp"
#include "simphonie/kern/Scheduler.hpp"
#include "simphonie/kern/Simulator.hpp"

namespace test {
using namespace simphonie::colibry;

// ----------------------------------------------------------
// test fixture implementation
ABS_TEST_SUITE_BEGIN( SimControl )

private:
    class ModelCounter : public simdeck::EPPModel {
    public:
        inline ModelCounter(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
            : EPPModel(name, descr, parent), _counter(0) {
            addEP("step", "Main model entry point", this, &ModelCounter::step);
        }
        inline void step() {
            logInfo("call");
            _counter++;
            _evntMgr->Emit(_eventId);
        };
        inline Smp::Int64 getCounter() const {
            return _counter;
        }

    private:
        inline void connect() {
            _evntMgr = getSimulator()->GetEventManager();
            _eventId = _evntMgr->QueryEventId("TheEvent");
            getSimulator()->GetScheduler()->AddSimulationTimeEvent(GetEntryPoint("step"), 0, 10, -1);
        }
        inline void publish(Smp::IPublication* receiver) {
            receiver->PublishField("counter", "", &_counter, Smp::ViewKind::VK_All, false, false, true);
        }

        Smp::Services::IEventManager* _evntMgr;
        Smp::Services::EventId _eventId;
        Smp::Int64 _counter;
    };

    simphonie::kern::Simulator* _sim;
    SimControl* _ctrl;
    ModelCounter* _incr;

public:
    void setUp() {
        _sim = new simphonie::kern::Simulator("TestSimControlSimu", "", nullptr);
        _ctrl = new SimControl("TestSimControlSimCtrl", "", _sim);
        _incr = new ModelCounter("TestSimControlMdlCounter", "", _sim);

        _sim->AddService(_ctrl);
        _sim->AddModel(_incr);
        _sim->Publish();
        _sim->Configure();
    }

    void tearDown() {
        delete _sim;
    }

    ABS_TEST_CASE_BEGIN(SimControlSimTime) {
        ABS_TEST_CASE_REQ(simph.simctrl.1)
        ABS_TEST_CASE_REQ(simph.expr.1)
        ABS_TEST_CASE_REQ(simph.expr.2)

        _ctrl->setExpression("(>= /TimeKeeper/simTime 80)");
        _sim->Connect();
        _sim->Run();
        while (_sim->GetState() == Smp::SimulatorStateKind::SSK_Executing) {}
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)80, _sim->GetTimeKeeper()->GetSimulationTime());
    }
    ABS_TEST_CASE_END

    ABS_TEST_CASE_BEGIN(SimControlEvent) {
        ABS_TEST_CASE_REQ(simph.simctrl.1)
        ABS_TEST_CASE_REQ(simph.expr.1)
        ABS_TEST_CASE_REQ(simph.expr.3)

        _ctrl->setExpression("(= TheEvent 10)");
        _sim->Connect();
        _sim->Run();
        while (_sim->GetState() == Smp::SimulatorStateKind::SSK_Executing) {}
        CPPUNIT_ASSERT_EQUAL((Smp::Int64)10, _incr->getCounter());
    }
    ABS_TEST_CASE_END

    ABS_TEST_CASE_BEGIN(SimControlField) {
        ABS_TEST_CASE_REQ(simph.simctrl.1)
        ABS_TEST_CASE_REQ(simph.expr.1)
        ABS_TEST_CASE_REQ(simph.expr.4)

        _ctrl->setExpression("(> /TestSimControlMdlCounter/counter 5)");
        _sim->Connect();
        _sim->Run();
        while (_sim->GetState() == Smp::SimulatorStateKind::SSK_Executing) {
            usleep(1000);
        }
        CPPUNIT_ASSERT_EQUAL((Smp::Int64)6, _incr->getCounter());
    }
    ABS_TEST_CASE_END

    ABS_TEST_SUITE_END
    } /* namespace test */
