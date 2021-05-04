/*
 * @file ExInvalidReturnValue.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExInvalidReturnValue.hpp"
#include <sstream>

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidReturnValue::ExInvalidReturnValue(Smp::IObject* sender, Smp::String8 opname, Smp::AnySimple value)
    : _opname(opname), _value(value) {
    setName("InvalidReturnValue");
    setSender(sender);
    std::ostringstream d;
    d << "Can't assign " << opname << ", invalid return value : " << value;
    setDescription(d.str().c_str());
    setMessage("");
}  //
// ..........................................................
ExInvalidReturnValue::~ExInvalidReturnValue() {}

Smp::String8 ExInvalidReturnValue::GetOperationName() const noexcept {
    return _opname;
}

Smp::AnySimple ExInvalidReturnValue::GetValue() const noexcept {
    return _value;
}

}  // namespace kern
}  // namespace simph
