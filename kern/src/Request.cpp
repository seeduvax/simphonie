/*
 * @file Request.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Request.hpp"
#include "simph/kern/ExInvalidAnyType.hpp"
#include "simph/kern/ExInvalidParameterIndex.hpp"
#include "simph/kern/ExInvalidParameterValue.hpp"
#include "simph/kern/ExVoidOperation.hpp"

namespace simph {
namespace kern {

// --------------------------------------------------------------------
// ..........................................................
Request::Request(Smp::String8 operationName, const std::vector<Parameter>& parameters,
                 Smp::PrimitiveTypeKind returnType)
    : _operationName(operationName), _returnValue(returnType), _parameters(parameters) {}
// ..........................................................
Request::~Request() {}
// ..........................................................
Smp::Int32 Request::GetParameterIndex(Smp::String8 parameterName) const {
    auto it = _parameters.begin();
    for (; it != _parameters.end(); ++it) {
        if (it->name == parameterName) {
            break;
        }
    }
    return it == _parameters.end() ? -1 : it - _parameters.begin();
}
// ..........................................................
void Request::SetParameterValue(Smp::Int32 index, Smp::AnySimple value) {
    if (index < 0 || index >= _parameters.size()) {
        throw ExInvalidParameterIndex(nullptr, _operationName.c_str(), index, _parameters.size());
    }
    else {
        auto& parameter = _parameters[index];
        if (parameter.value.type != value.type) {
            throw ExInvalidParameterValue(nullptr, parameter.name.c_str(), value);
        }
        else {
            parameter.value = value;
        }
    }
}
// ..........................................................
Smp::AnySimple Request::GetParameterValue(Smp::Int32 index) const {
    if (index < 0 || index >= _parameters.size()) {
        throw ExInvalidParameterIndex(nullptr, _operationName.c_str(), index, _parameters.size());
    }
    return _parameters[index].value;
}
// ..........................................................
void Request::SetReturnValue(Smp::AnySimple value) {
    if (_returnValue.type == Smp::PrimitiveTypeKind::PTK_None) {
        throw ExVoidOperation(nullptr, _operationName.c_str());
    }
    else if (_returnValue.type != value.type) {
        throw ExInvalidAnyType(nullptr, value.type, _returnValue.type);
    }
    // TODO: throw  Smp::InvalidReturnValue
    // (Astyl) what are invalid value cases ?
    _returnValue = value;
}
// ..........................................................
Smp::AnySimple Request::GetReturnValue() const {
    if (_returnValue.type == Smp::PrimitiveTypeKind::PTK_None) {
        throw ExVoidOperation(nullptr, _operationName.c_str());
    }
    return _returnValue;
}

}  // namespace kern
}  // namespace simph
