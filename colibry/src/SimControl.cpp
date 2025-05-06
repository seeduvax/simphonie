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
#include "simdeck/EntryPoint.hpp"
#include "simdeck/ExInvalidAnyType.hpp"
#include "simdeck/ExInvalidFieldName.hpp"
#include "simdeck/Exception.hpp"
#include "simdeck/StringField.hpp"
#include "simphonie/kern/EventManager.hpp"
#include "simphonie/kern/Resolver.hpp"

#define CHECK_EP_NAME "checkStop"

namespace simphonie {
namespace colibry {

SimControl::SimControl(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simdeck::Service(name, descr, parent) {
    _condition = "(not true)";
    _evaluator = nullptr;
    addEP(CHECK_EP_NAME, "Check stop condition and request simulation hold when condition is met.", this,
          &SimControl::_checkStopCondition);
}

void SimControl::publish(Smp::IPublication* receiver) {
    receiver->PublishField(simdeck::StringField::Create(
        "condition", "S-expression condition. Service shall stop simulation when the condition is no longer false.",
        Smp::ViewKind::VK_All, &_condition, nullptr, false, true, false, this));
}

void SimControl::_checkStopCondition() {
    const T val = _evaluator->evaluate();
    if (std::abs(val) > std::numeric_limits<T>::epsilon()) {
        getSimulator()->GetLogger()->Log(this, "Request simulation stop", Smp::Services::ILogger::LMK_Information);
        getSimulator()->Hold(true);
    }
}

void SimControl::connect() {
    auto resolver = [&](const char* name) -> std::function<T(void)> {
        Smp::IObject* obj = getSimulator()->GetResolver()->ResolveAbsolute(name);
        if (obj == nullptr) {
            std::stringstream ss;
            ss << "Path \"" << name << "\" led to nothing";
            throw simdeck::ExInvalidFieldName(this, ss.str().c_str());
        }
        Smp::ISimpleField* field;
        try {
            field = dynamic_cast<Smp::ISimpleField*>(obj);
        }
        catch (const std::exception& e) {
            std::stringstream ss;
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

    try {
        _evaluator = std::make_unique<sxeval::SXEval<T> >(const_cast<char*>(_condition.c_str()), resolver);
    }
    catch (const std::exception& e) {
        std::stringstream ss;
        ss << "Fatal error during SimControl::_evaluator's instanciation: " << e.what();
        getSimulator()->GetLogger()->Log(this, ss.str().c_str(), Smp::Services::ILogger::LMK_Error);
        throw simdeck::Exception(this, ss.str().c_str());
    }

    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_PostSimTimeChangeId,
                                                 GetEntryPoint(CHECK_EP_NAME));
}

} /* namespace colibry */
} /* namespace simphonie */
