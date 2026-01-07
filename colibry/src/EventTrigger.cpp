/*
 * @file EventTrigger.cpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/colibry/EventTrigger.hpp"
#include "simdeck/StringType.hpp"
#include "Smp/IPublication.h"
#include "Smp/ISimulator.h"

namespace simphonie {
namespace colibry {
// --------------------------------------------------------------------
// ..........................................................
EventTrigger::EventTrigger(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent ):
            Parent(name, descr, parent) {
}
// ..........................................................
EventTrigger::~EventTrigger() {
}
// --------------------------------------------------------------------
// ..........................................................
void EventTrigger::publish(Smp::IPublication* receiver) {
    Parent::publish(receiver);
    receiver->PublishField(
        "OnTrueEvent", "Name of the event to trig when the expression result is true",
        &_onTrueEventName, simdeck::StringType::UuidString, Smp::ViewKind::VK_All, false, true, false);
    receiver->PublishField(
        "OnBecomeTrueEvent", "Name of the event to trig when the expression result is switched from false to true",
        &_onBecomeTrueEventName, simdeck::StringType::UuidString, Smp::ViewKind::VK_All, false, true, false);
    receiver->PublishField(
        "OnBecomeFalseEvent", "Name of the event to trig when the expression result is switched from true to false",
        &_onBecomeFalseEventName, simdeck::StringType::UuidString, Smp::ViewKind::VK_All, false, true, false);
    receiver->PublishField(
        "OnChangedEvent", "Name of the event to trig when the expression result changed whatever the change is.",
        &_onChangedEventName, simdeck::StringType::UuidString, Smp::ViewKind::VK_All, false, true, false);
}
// ..........................................................
void EventTrigger::connect() {
    Parent::connect();
    _eventManager=getSimulator()->GetEventManager();
    if (_onTrueEventName!="") {
        _onTrueEventId = _eventManager->QueryEventId(_onTrueEventName.c_str());
    }
    if (_onBecomeTrueEventName!="") {
        _onBecomeTrueEventId = _eventManager->QueryEventId(_onBecomeTrueEventName.c_str());
    }
    if (_onBecomeFalseEventName!="") {
        _onBecomeFalseEventId = _eventManager->QueryEventId(_onBecomeFalseEventName.c_str());
    }
    if (_onChangedEventName!="") {
        _onChangedEventId = _eventManager->QueryEventId(_onChangedEventName.c_str());
    }
}
// --------------------------------------------------------------------
// ..........................................................
void EventTrigger::onEvaluate() {
    if ( _onTrueEventId!=-1 && getOut()!=0 ) {
        _eventManager->Emit(_onTrueEventId);
    }
    if ( _onBecomeTrueEventId!=-1 && getChanged() && getOut()!=0 ) {
        _eventManager->Emit(_onBecomeTrueEventId);
    }
    if ( _onBecomeFalseEventId!=-1 && getChanged() && getOut()==0 ) {
        _eventManager->Emit(_onBecomeFalseEventId);
    }
    if ( _onChangedEventId!=-1 && getChanged() ) {
        _eventManager->Emit(_onChangedEventId);
    }
}

}} // namespace simphonie::colibry
