/*
 * @file Operation.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/Operation.hpp"
#include "simdeck/Exception.hpp"
#include "Smp/IRequest.h"

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
Operation::Operation(Smp::String8 name, Smp::String8 description, Smp::IObject* parent)
    : simdeck::Object(name, description, parent), _params("params", "", this) {}
// ..........................................................
Operation::~Operation() {}
// --------------------------------------------------------------------
// ..........................................................
const Smp::ParameterCollection* Operation::GetParameters() const {
    return &_params;
}
// ..........................................................
Smp::IParameter* Operation::GetParameter(Smp::String8 name) const {
    return _params.at(name);
}
// ..........................................................
Smp::IParameter* Operation::GetReturnParameter() const {
    return _retParam;
}
// ..........................................................
Smp::ViewKind Operation::GetView() const {
    return _view;
}
// ..........................................................
Smp::IRequest* Operation::CreateRequest() {
    // TODO
    throw Exception(this, "Operation::CreateRequest() not implemented yet!");
    return nullptr;
}
// ..........................................................
void Operation::Invoke(Smp::IRequest* request) {
    // TODO
    throw Exception(this, "Operation::Invoke(Smp::IRequest*) not implemented yet!");
}
// ..........................................................
void Operation::DeleteRequest(Smp::IRequest* request) {
    if (request != nullptr) {
        // TODO 
        throw Exception(this, "Operation::DeleteRequest(Smp::IRequest*) not implemented yet!");
//        delete request;
    }
}

}  // namespace simdeck
