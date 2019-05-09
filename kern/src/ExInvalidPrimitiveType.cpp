/*
 * @file ExInvalidPrimitiveType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExInvalidPrimitiveType.hpp"
#include "simph/kern/TypeRegistry.hpp"
#include <sstream>

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidPrimitiveType::ExInvalidPrimitiveType(Smp::IObject* sender, 
                Smp::PrimitiveTypeKind type): _type(type) {
    setName("InvalidPrimitiveType");
    setSender(sender);
    std::ostringstream d;
    d << "Invalid primitive type: "<<_type;
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidPrimitiveType::~ExInvalidPrimitiveType() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExInvalidPrimitiveType::GetTypeName() const noexcept {
    return TypeRegistry::getPrimitiveTypeName(_type);
}
// ..........................................................
Smp::PrimitiveTypeKind ExInvalidPrimitiveType::GetType() const noexcept {
    return _type;
}
}} // namespace simph::kern
