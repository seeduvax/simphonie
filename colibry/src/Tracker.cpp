/*
 * @file Tracker.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/colibry/Tracker.hpp"
#include "Smp/ISimpleField.h"
#include "Smp/ISimulator.h"
#include "Smp/IPublication.h"
#include "Smp/Services/IResolver.h"
#include "Smp/Services/IEventManager.h"
#include "simdeck/StringType.hpp"
#include "simdeck/ExInvalidFieldName.hpp"
#include "simdeck/ExInvalidAnyType.hpp"

#define INIT_EP_NAME "Init"
#define CONTAINER_NAME "EventCounters"

namespace simphonie {
namespace colibry {
// --------------------------------------------------------------------
// ..........................................................
Tracker::Tracker(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
        Parent(name, descr, parent) {
    addEP(EVALUATE_EP_NAME, "Run the evaluation of the expression", this,
            &Tracker::epEvaluate);
    addEP(INIT_EP_NAME, "Build expression evaluation.", this,
            &Tracker::epInit);
    addContainer(CONTAINER_NAME, "Event emits counter");
}
// ..........................................................
Tracker::~Tracker() {
    for (auto evc: *(GetContainer(CONTAINER_NAME)->GetComponents())) {
        delete evc;
    }
}
// --------------------------------------------------------------------
// ..........................................................
void Tracker::publish(Smp::IPublication* receiver) {
    receiver->PublishField(
        "Expression", "S-expression to be evaluated by this service.",
        &_expression, simdeck::StringType::UuidString, Smp::ViewKind::VK_All, false, true, false);
    receiver->PublishField(
        "Out", "S-expression evaluation result.",
        &_out, Smp::ViewKind::VK_All, false, false, true);
    receiver->PublishField(
        "Changed", "S-expression evaluation result change since previous evaluation.",
        &_changed, Smp::ViewKind::VK_All, false, false, true);
    receiver->PublishField(
        "ChangedCount", "S-expression evaluation result changes count since simulation start.",
        &_changedCount, Smp::ViewKind::VK_All, false, false, true);
}
// ..........................................................
void Tracker::connect() {
    getSimulator()->AddInitEntryPoint(GetEntryPoint(INIT_EP_NAME));
}
// ..........................................................
void Tracker::epInit() {
    auto encapsulatedResolver = [&](const std::string& name) -> std::function<Smp::Float64(void)> {
        Smp::IObject* obj = getSimulator()->GetResolver()->ResolveAbsolute(name.c_str());
        if (obj == nullptr) {
            std::ostringstream ss;
            ss << "Path \"" << name << "\" led to nothing";
            throw simdeck::ExInvalidFieldName(this, ss.str().c_str());
        }
        Smp::ISimpleField* field;
        try {
            field = dynamic_cast<Smp::ISimpleField*>(obj);
        }
        catch (const std::exception& e) {
            std::ostringstream ss;
            ss << "Failed to convert to Smp::ISimpleField: " << name << ": " << e.what();
            throw simdeck::ExInvalidFieldName(this, ss.str().c_str());
        }
        switch (field->GetValue().GetType()) {
            case Smp::PrimitiveTypeKind::PTK_Char8:
                return [=]() { return static_cast<Smp::Float64>(field->GetValue().value.char8Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Bool:
                return [=]() { return static_cast<Smp::Float64>(field->GetValue().value.boolValue); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Int8:
                return [=]() { return static_cast<Smp::Float64>(field->GetValue().value.int8Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt8:
                return [=]() { return static_cast<Smp::Float64>(field->GetValue().value.uInt8Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Int16:
                return [=]() { return static_cast<Smp::Float64>(field->GetValue().value.int16Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt16:
                return [=]() { return static_cast<Smp::Float64>(field->GetValue().value.uInt16Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Int32:
                return [=]() { return static_cast<Smp::Float64>(field->GetValue().value.int32Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt32:
                return [=]() { return static_cast<Smp::Float64>(field->GetValue().value.uInt32Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Int64:
                return [=]() { return static_cast<Smp::Float64>(field->GetValue().value.int64Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt64:
                return [=]() { return static_cast<Smp::Float64>(field->GetValue().value.uInt64Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Float32:
                return [=]() { return static_cast<Smp::Float64>(field->GetValue().value.float32Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Float64:
                return [=]() { return static_cast<Smp::Float64>(field->GetValue().value.float64Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Duration:
                return [=]() { return static_cast<Smp::Float64>(field->GetValue().value.durationValue); };
                break;
            case Smp::PrimitiveTypeKind::PTK_DateTime:
                return [=]() { return static_cast<Smp::Float64>(field->GetValue().value.dateTimeValue); };
                break;
            case Smp::PrimitiveTypeKind::PTK_String8:
            case Smp::PrimitiveTypeKind::PTK_None:
            default:
                throw simdeck::ExInvalidAnyType(this, field->GetValue(), Smp::PrimitiveTypeKind::PTK_None);
                break;
        }
    };

    auto ownResolver = [&](const std::string& name) -> Smp::Float64& {
        {
            const auto obj = getSimulator()->GetResolver()->ResolveAbsolute(name.c_str());
            if (obj != nullptr) {
                /* throw an error to force encapsulated variable resolution */
                throw std::runtime_error("This is a field (encapsulated variable) instead of an event.");
            }
        }
        auto evCounter=dynamic_cast<EventCounter*>(GetContainer(CONTAINER_NAME)->GetComponent(name.c_str()));
        if ( evCounter == nullptr ) {
            const auto eventId = getSimulator()->GetEventManager()->QueryEventId(name.c_str());
            evCounter=new EventCounter(name.c_str(), "Event counter", this, eventId);
            GetContainer(CONTAINER_NAME)->AddComponent(evCounter);
        }
        return evCounter->get();
    };

    try {
        _evaluator.build(_expression, ownResolver, encapsulatedResolver);
        std::ostringstream ss;
        ss << "Condition updated to \"" << _expression << "\"";
        logInfo(ss.str().c_str());
    }
    catch (const std::exception& e) {
        std::ostringstream ss;
        ss << "Fatal error during the S-expression evaluator building: " << e.what();
        throw simdeck::Exception(this, ss.str().c_str());
    }
}

// --------------------------------------------------------------------
// ..........................................................
void Tracker::onEvaluate() {
}


// ..........................................................
void Tracker::epEvaluate() {
    auto out = _evaluator.execute();
    if (out != _out) {
        _changed=true;
        _changedCount++;
    }
    else {
        _changed=false;
    }
    _out = out;
    onEvaluate();
}

// --------------------------------------------------------------------
// ..........................................................
Tracker::EventCounter::EventCounter(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent,
                                         Smp::Services::EventId eventId)
    : simdeck::Component(name, descr, parent), _counter(0) {
    addEP("handler", "Internal use only.", this, &EventCounter::handle);
    dynamic_cast<Tracker*>(parent)->getSimulator()->GetEventManager()->Subscribe(eventId, GetEntryPoint("handler"));
}

}} // namespace simphonie::colibry
