/*
 * @file ExInvalidPrimitiveType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/ExInvalidPrimitiveType.hpp"
#include <sstream>

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidPrimitiveType::ExInvalidPrimitiveType(const Smp::IObject* sender, Smp::PrimitiveTypeKind type) : _type(type) {
    setName("InvalidPrimitiveType");
    setSender(sender);
    std::ostringstream d;
    d << "Invalid primitive type: " << _type;
    setDescription(d.str().c_str());
    setMessage();
    std::ostringstream ss;
    ss << _type;
    _typeName=ss.str();
}
// ..........................................................
ExInvalidPrimitiveType::~ExInvalidPrimitiveType() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExInvalidPrimitiveType::GetTypeName() const noexcept {
    return _typeName.c_str();
}
// ..........................................................
Smp::PrimitiveTypeKind ExInvalidPrimitiveType::GetType() const noexcept {
    return _type;
}
}  // namespace simdeck
