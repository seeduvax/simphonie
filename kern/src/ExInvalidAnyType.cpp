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
ExInvalidAnyType::ExInvalidAnyType(Smp::IObject* sender, Smp::PrimitiveTypeKind invalid,
                                   Smp::PrimitiveTypeKind expected)
    : _invalid(invalid), _expected(expected) {
    setName("InvalidAnyType");
    setSender(sender);
    std::ostringstream d;
    d << "Invalid type " << invalid << ", " << expected << " expected.";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidAnyType::~ExInvalidAnyType() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::PrimitiveTypeKind ExInvalidAnyType::GetInvalidType() const noexcept {
    return _invalid;
}
// ..........................................................
Smp::PrimitiveTypeKind ExInvalidAnyType::GetExpectedType() const noexcept {
    return _expected;
}

}  // namespace kern
}  // namespace simph
