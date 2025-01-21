/*
 * @file ExInvalidPrimitiveType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/ExInvalidPrimitiveType.hpp"
#include <sstream>

namespace simph {
namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidPrimitiveType::ExInvalidPrimitiveType(Smp::IObject* sender, Smp::PrimitiveTypeKind type) : _type(type) {
    setName("InvalidPrimitiveType");
    setSender(sender);
    std::ostringstream d;
    d << "Invalid primitive type: " << _type;
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidPrimitiveType::~ExInvalidPrimitiveType() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExInvalidPrimitiveType::GetTypeName() const noexcept {
// TODO can't do that, this involves a singleton!    return TypeRegistry::getPrimitiveTypeName(_type);
return "name_not_retrieved";
}
// ..........................................................
Smp::PrimitiveTypeKind ExInvalidPrimitiveType::GetType() const noexcept {
    return _type;
}
}  // namespace smpdk
}  // namespace simph
