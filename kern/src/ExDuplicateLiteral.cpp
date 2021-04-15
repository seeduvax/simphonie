/*
 * @file ExDuplicateLiteral.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExDuplicateLiteral.hpp"
#include <sstream>

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExDuplicateLiteral::ExDuplicateLiteral(Smp::IObject* sender, Smp::String8 name, Smp::Int32 value)
    : _lName(name), _lValue(value) {
    setSender(sender);
    setName("DuplicateLiteral");
    std::ostringstream d;
    d << "Literal " << value << " already registered as " << name;
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExDuplicateLiteral::~ExDuplicateLiteral() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExDuplicateLiteral::GetLiteralName() const noexcept {
    return _lName;
}
// ..........................................................
Smp::Int32 ExDuplicateLiteral::GetLiteralValue() const noexcept {
    return _lValue;
}
}  // namespace kern
}  // namespace simph
