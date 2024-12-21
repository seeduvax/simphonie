/*
 * @file ExInvalidParameterValue.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExInvalidParameterValue.hpp"
#include <sstream>

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidParameterValue::ExInvalidParameterValue(Smp::IObject* sender, Smp::String8 opName, Smp::String8 paramName)
    : _opName(opName), _paramName(paramName) {
    setName("InvalidParameterValue");
    setSender(sender);
    std::ostringstream d;
    d << "Invalid value for parameter " << paramName << " from operation" << opName;
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidParameterValue::~ExInvalidParameterValue() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExInvalidParameterValue::GetOperationName() const noexcept {
    return _opName;
}
// ..........................................................
Smp::String8 ExInvalidParameterValue::GetParameterName() const noexcept {
    return _paramName;
}

}  // namespace kern
}  // namespace simph
