/*
 * @file Operation.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Operation.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Operation::Operation(Smp::String8 name, Smp::String8 description,
                        Smp::IObject* parent):
                simph::smpdk::Object(name,description,parent),
                _params("params","",this) {
        
}
// ..........................................................
Operation::~Operation() {
}
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
LOGE("Operation::CreateRequest() not implemented yet!")
return nullptr;
}
// ..........................................................
void Operation::Invoke(Smp::IRequest* request) {
LOGE("Operation::Invoke(Smp::IRequest*) not implemented yet!")
}
// ..........................................................
void Operation::DeleteRequest(Smp::IRequest* request) {
    if (request!=nullptr) {
        // TODO should be checked here the given request was created from
        // this operation instance...
        delete request;
    }
}

}} // namespace simph::kern
