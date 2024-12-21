/*
 * @file ExInvalidAnyType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExInvalidAnyType.hpp"
#include <sstream>

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidAnyType::ExInvalidAnyType(Smp::IObject* sender,
                                   Smp::AnySimple invalidValue,
                                   Smp::PrimitiveTypeKind expected)
    : _invalidValue(invalidValue), _expected(expected) {
    setName("InvalidAnyType");
    setSender(sender);
    std::ostringstream d;
    d << "Invalid type for value " << invalidValue << ", " << expected << " expected.";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidAnyType::~ExInvalidAnyType() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::AnySimple ExInvalidAnyType::GetInvalidValue() const noexcept {
    return _invalidValue;
}
// ..........................................................
Smp::PrimitiveTypeKind ExInvalidAnyType::GetExpectedType() const noexcept {
    return _expected;
}

}  // namespace kern
}  // namespace simph
