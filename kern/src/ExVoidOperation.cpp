/*
 * @file ExVoidOperation.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExVoidOperation.hpp"
#include <sstream>

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExVoidOperation::ExVoidOperation(Smp::IObject* sender, Smp::String8 opName):
                _opName(opName) {
    setName("VoidOperation");
    setSender(sender);
    std::ostringstream d;
    d << "Operation "<<opName<<" return type is void.";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExVoidOperation::~ExVoidOperation() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExVoidOperation::GetOperationName() const noexcept {
    return _opName;
}

}} // namespace simph::kern
