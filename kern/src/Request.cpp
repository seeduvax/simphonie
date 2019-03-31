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

namespace simph {
	namespace kern {

// --------------------------------------------------------------------
// ..........................................................
Request::Request( Smp::String8 operationName,
                  const std::vector<Parameter>& parameters,
                  Smp::PrimitiveTypeKind returnType):
        _operationName(operationName),
        _returnValue(returnType),
        _parameters(parameters) {
}
// ..........................................................
Request::~Request() {
}
// ..........................................................
Smp::Int32 Request::GetParameterIndex( Smp::String8 parameterName) const {
    auto it = _parameters.begin();
    for (;it!=_parameters.end();++it) {
        if (it->name == parameterName) {
            break;
        }
    }
    return it==_parameters.end() ? -1 : it - _parameters.begin();
}
// ..........................................................
void Request::SetParameterValue(Smp::Int32 index, Smp::AnySimple value) {
    if (index<0 || index >= _parameters.size()) {
        //TODO throw Smp::InvalidParameterIndex;
    } else {
        auto& parameter = _parameters[index];
        if (parameter.value.type != value.type ) {
            //TODO throw Smp::InvalidParameterValue;
        } else {
            parameter.value = value;
        }
    }
}
// ..........................................................
Smp::AnySimple Request::GetParameterValue(Smp::Int32 index) const {
    if (index<0 || index >= _parameters.size()) {
        //TODO throw Smp::InvalidParameterIndex;
    }
    return _parameters[index].value;
}
// ..........................................................
void Request::SetReturnValue(Smp::AnySimple value) {
    if (_returnValue.type == Smp::PrimitiveTypeKind::PTK_None) {
        //TODO throw Smp::VoidOperation;
    } else if (_returnValue.type != value.type) {
        //TODO throw Smp::InvalidAnyType;
    }
    // TODO: throw  Smp::InvalidReturnValue
    // (Astyl) what are invalid value cases ?
    _returnValue = value;
}
// ..........................................................
Smp::AnySimple Request::GetReturnValue() const {
    if (_returnValue.type == Smp::PrimitiveTypeKind::PTK_None) {
        //TODO throw Smp::VoidOperation;
    }
    return _returnValue;
}

}} // namespace simph::kern
