/*
 * @file TestEventTrigger.cpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "abs/test.h"
#include "simphonie/colibry/EventTrigger.hpp"
#include "simphonie/kern/Simulator.hpp"
#include "simdeck/EPPModel.hpp"
#include "Smp/Services/ITimeKeeper.h"
#include "Smp/Services/IScheduler.h"
#include "Smp/IPublication.h"
#include "Smp/ISimpleField.h"
 
namespace test {
using namespace simphonie::colibry;
 
// ----------------------------------------------------------
// test suite implementation
ABS_TEST_SUITE_BEGIN( EventTrigger )
 
private:
    class Model: public simdeck::EPPModel {
    public:
        Model(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
                simdeck::EPPModel(name, descr, parent) {
            addEP("step", "counter increment", this, &Model::step);
            addEP("onTrue", "True event handler", this, &Model::onTrue);
            addEP("onBecomeTrue", "BecomeTrue event handler", this, &Model::onBecomeTrue);
            addEP("onBecomeFalse", "BecomeFalse event handler", this, &Model::onBecomeFalse);
            addEP("onChanged", "Changed event handler", this, &Model::onChanged);
        }
        virtual ~Model() {
        }
        void step() {
            if ( _stepCount%5 == 0 ) {
                // arbitrary lower counter to check on changed event on non
                // boolean expression.
                _subStepCount++;
            }
            _stepCount++;
        }
        void onTrue() {
            _onTrueCount++;
        }
        void onBecomeTrue() {
            _onBecomeTrueCount++;
            _onBecomeTrueDate=getSimulator()->GetTimeKeeper()->GetSimulationTime();
        }
        void onBecomeFalse() {
            _onBecomeFalseCount++;
            _onBecomeFalseDate=getSimulator()->GetTimeKeeper()->GetSimulationTime();
        }
        void onChanged() {
            _onChangedCount++;
        }

        Smp::Int32 _onTrueCount=0;
        Smp::Int32 _onBecomeTrueCount=0;
        Smp::Int32 _onBecomeFalseCount=0;
        Smp::Int32 _onChangedCount=0;
        Smp::Int32 _stepCount=0;
        Smp::Int32 _subStepCount=0;
        Smp::Duration _onBecomeTrueDate=0;
        Smp::Duration _onBecomeFalseDate=0;
    protected:
        void publish(Smp::IPublication*  receiver) override {
            receiver->PublishField("count", "Step count", &_stepCount,
                Smp::ViewKind::VK_All, false, false, true);
            receiver->PublishField("subCount", "Sub step count", &_subStepCount,
                Smp::ViewKind::VK_All, false, false, true);
        }
        void connect() override {
            // activate step 30 times.
            getSimulator()->GetScheduler()->AddSimulationTimeEvent(GetEntryPoint("step"), 0, 10, 29);
            // subscribe events
            auto evMgr=getSimulator()->GetEventManager();
            evMgr->Subscribe(
                evMgr->QueryEventId("TrueEvent"),
                GetEntryPoint("onTrue"));
            evMgr->Subscribe(
                evMgr->QueryEventId("BecomeTrueEvent"),
                GetEntryPoint("onBecomeTrue"));
            evMgr->Subscribe(
                evMgr->QueryEventId("BecomeFalseEvent"),
                GetEntryPoint("onBecomeFalse"));
            evMgr->Subscribe(
                evMgr->QueryEventId("ChangedEvent"),
                GetEntryPoint("onChanged"));
        }
    };

    simphonie::kern::Simulator* _sim;
    EventTrigger* _eventTrigger;
    Model* _model;
 
public:
    void setStringField(     Smp::IComponent* comp,
                             Smp::String8 fieldName,
                             Smp::String8 fieldValue) {
        auto f = dynamic_cast<Smp::ISimpleField*>(comp->GetField(fieldName));
        CPPUNIT_ASSERT(f!=nullptr);
        f->SetValue({Smp::PrimitiveTypeKind::PTK_String8, fieldValue});
    }
    void setUp() {
        _sim = new simphonie::kern::Simulator("TestSimulator", "", nullptr);
        _eventTrigger = new EventTrigger("EventTrigger", "", _sim);
        _sim->AddService(_eventTrigger);
        _model = new Model("model", "", _sim);
        _sim->AddModel(_model);

        _sim->Publish();
        _sim->Configure();
    }
 
    void tearDown() {
        delete _sim;
    }
 
    ABS_TEST_CASE_BEGIN(DistinctEvents) {
        ABS_TEST_DESCR(Check event trigger emit distinct events on specific condition and condition changes)
        ABS_TEST_CASE_REQ(req.id) 
        setStringField(_eventTrigger, "Expression", "(and (>= /model/count 10) (< /model/count 20))");
        setStringField(_eventTrigger, "OnTrueEvent", "TrueEvent");
        setStringField(_eventTrigger, "OnBecomeTrueEvent", "BecomeTrueEvent");
        setStringField(_eventTrigger, "OnBecomeFalseEvent", "BecomeFalseEvent");
        setStringField(_eventTrigger, "OnChangedEvent", "ChangedEvent");

        _sim->GetEventManager()->Subscribe(
                Smp::Services::IEventManager::SMP_PostSimTimeChangeId,
                _eventTrigger->GetEntryPoint("Evaluate"));

        _sim->Connect();
        _sim->Run();
        while (_sim->GetState() == Smp::SimulatorStateKind::SSK_Executing) {
            usleep(1000);
        }
        CPPUNIT_ASSERT_EQUAL(30,_model->_stepCount);
        CPPUNIT_ASSERT_EQUAL(10,_model->_onTrueCount);
        CPPUNIT_ASSERT_EQUAL(1,_model->_onBecomeTrueCount);
        CPPUNIT_ASSERT_EQUAL(1,_model->_onBecomeFalseCount);
        CPPUNIT_ASSERT_EQUAL(2,_model->_onChangedCount);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)100,_model->_onBecomeTrueDate);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)200,_model->_onBecomeFalseDate);
    }
    ABS_TEST_CASE_END

    ABS_TEST_CASE_BEGIN(NotBooleanExpression) {
        ABS_TEST_DESCR(Check event trigger emit on change condition of a non boolean result)
        ABS_TEST_CASE_REQ(req.id) 
        setStringField(_eventTrigger, "Expression", "(+ /model/subCount 0)");
        setStringField(_eventTrigger, "OnChangedEvent", "ChangedEvent");

        _sim->GetEventManager()->Subscribe(
                Smp::Services::IEventManager::SMP_PostSimTimeChangeId,
                _eventTrigger->GetEntryPoint("Evaluate"));

        _sim->Connect();
        _sim->Run();
        while (_sim->GetState() == Smp::SimulatorStateKind::SSK_Executing) {
            usleep(1000);
        }
        CPPUNIT_ASSERT_EQUAL(30,_model->_stepCount);
        CPPUNIT_ASSERT_EQUAL(0,_model->_onTrueCount);
        CPPUNIT_ASSERT_EQUAL(0,_model->_onBecomeTrueCount);
        CPPUNIT_ASSERT_EQUAL(0,_model->_onBecomeFalseCount);
        CPPUNIT_ASSERT_EQUAL(6,_model->_onChangedCount);
    }
    ABS_TEST_CASE_END

ABS_TEST_SUITE_END
} // namespace test
