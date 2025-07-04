/*
 * @file SimControl.cpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/colibry/SimControl.hpp"

#include <functional>
#include <iomanip>
#include <limits>
#include <string>

#include "Smp/IPublication.h"
#include "Smp/ISimpleField.h"
#include "Smp/Services/IEventManager.h"
#include "Smp/Services/IResolver.h"
#include "simdeck/EntryPoint.hpp"
#include "simdeck/ExInvalidAnyType.hpp"
#include "simdeck/ExInvalidFieldName.hpp"
#include "simdeck/Exception.hpp"
#include "simdeck/StringType.hpp"

#define CHECK_EP_NAME "checkStop"
#define CONTAINER_NAME "EventHandlers"

namespace simphonie {
namespace colibry {

SimControl::SimControl(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simdeck::Service(name, descr, parent) {
    _condition = "(not true)";
    _evaluator = nullptr;
    addEP(CHECK_EP_NAME, "Check stop condition and request simulation hold when condition is met.", this,
          &SimControl::_checkStopCondition);
    addEP("applyCondition", "Apply the stopping s-expression condition.", this, &SimControl::applyCondition);
    addContainer(CONTAINER_NAME, "Internal use only.");
}

SimControl::~SimControl() {
    for (auto& e : _eventHandlers) {
        GetContainer(CONTAINER_NAME)->DeleteComponent(e.release());
    }
}

void SimControl::publish(Smp::IPublication* receiver) {
    receiver->PublishField(
        "condition", "S-expression condition. Service shall stop simulation when the condition is no longer false.",
        &_condition, simdeck::StringType::UuidString, Smp::ViewKind::VK_All, false, true, false);
}

void SimControl::applyCondition() {
    auto encapsulatedResolver = [&](const char* name) -> std::function<T(void)> {
        Smp::IObject* obj = getSimulator()->GetResolver()->ResolveAbsolute(name);
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
                return [=]() { return static_cast<T>(field->GetValue().value.char8Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Bool:
                return [=]() { return static_cast<T>(field->GetValue().value.boolValue); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Int8:
                return [=]() { return static_cast<T>(field->GetValue().value.int8Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt8:
                return [=]() { return static_cast<T>(field->GetValue().value.uInt8Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Int16:
                return [=]() { return static_cast<T>(field->GetValue().value.int16Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt16:
                return [=]() { return static_cast<T>(field->GetValue().value.uInt16Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Int32:
                return [=]() { return static_cast<T>(field->GetValue().value.int32Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt32:
                return [=]() { return static_cast<T>(field->GetValue().value.uInt32Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Int64:
                return [=]() { return static_cast<T>(field->GetValue().value.int64Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt64:
                return [=]() { return static_cast<T>(field->GetValue().value.uInt64Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Float32:
                return [=]() { return static_cast<T>(field->GetValue().value.float32Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Float64:
                return [=]() { return static_cast<T>(field->GetValue().value.float64Value); };
                break;
            case Smp::PrimitiveTypeKind::PTK_Duration:
                return [=]() { return static_cast<T>(field->GetValue().value.durationValue); };
                break;
            case Smp::PrimitiveTypeKind::PTK_DateTime:
                return [=]() { return static_cast<T>(field->GetValue().value.dateTimeValue); };
                break;
            case Smp::PrimitiveTypeKind::PTK_String8:
            case Smp::PrimitiveTypeKind::PTK_None:
            default:
                throw simdeck::ExInvalidAnyType(this, field->GetValue(), Smp::PrimitiveTypeKind::PTK_None);
                break;
        }
    };

    auto ownResolver = [&](const char* name) -> T& {
        {
            Smp::IObject* obj = getSimulator()->GetResolver()->ResolveAbsolute(name);
            if (obj != nullptr) {
                /* throw an error to force encapsulated variable resolution */
                throw std::runtime_error("This is a field (encapsulated variable) instead of an event.");
            }
        }
        const auto eventId = getSimulator()->GetEventManager()->QueryEventId(name);
        _eventHandlers.push_back(std::make_unique<_EventHandler>(name, "Internal use only.", this, eventId));
        GetContainer(CONTAINER_NAME)->AddComponent(_eventHandlers.back().get());
        return _eventHandlers.back()->get();
    };

    try {
        _evaluator.reset();
        _evaluator = std::make_unique<sxeval::SXEval<T> >(const_cast<char*>(_condition.c_str()), ownResolver,
                                                          encapsulatedResolver);
        std::ostringstream ss;
        ss << "Condition updated to \"" << _condition << "\"";
        logInfo(ss.str().c_str());
    }
    catch (const std::exception& e) {
        std::ostringstream ss;
        ss << "Fatal error during SimControl::_evaluator's instanciation: " << e.what();
        throw simdeck::Exception(this, ss.str().c_str());
    }
}

void SimControl::_checkStopCondition() {
    const T val = _evaluator->evaluate();
    if (std::abs(val) > std::numeric_limits<T>::epsilon()) {
        logInfo("Request simulation stop");
        getSimulator()->Hold(true);
    }
}

void SimControl::connect() {
    applyCondition();
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_PostSimTimeChangeId,
                                                 GetEntryPoint(CHECK_EP_NAME));
}

SimControl::_EventHandler::_EventHandler(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent,
                                         Smp::Services::EventId eventId)
    : Component(name, descr, parent), _counter(0) {
    addEP("handler", "Internal use only.", this, &_EventHandler::handle);
    dynamic_cast<SimControl*>(parent)->getSimulator()->GetEventManager()->Subscribe(eventId, GetEntryPoint("handler"));
}

} /* namespace colibry */
} /* namespace simphonie */
