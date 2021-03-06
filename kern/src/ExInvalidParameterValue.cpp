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
ExInvalidParameterValue::ExInvalidParameterValue(Smp::IObject* sender, Smp::String8 pName, Smp::AnySimple value)
    : _pName(pName), _value(value) {
    setName("InvalidParameterValue");
    setSender(sender);
    std::ostringstream d;
    d << "Invalid value for parameter " << pName << " = " << value;
    setDescription(d.str().c_str());
    // TODO Pourquoi setMessage() ne fonctionne pas sans argument?
    setMessage("");
}
// ..........................................................
ExInvalidParameterValue::~ExInvalidParameterValue() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExInvalidParameterValue::GetParameterName() const noexcept {
    return _pName;
}
// ..........................................................
Smp::AnySimple ExInvalidParameterValue::GetValue() const noexcept {
    return _value;
}

}  // namespace kern
}  // namespace simph
