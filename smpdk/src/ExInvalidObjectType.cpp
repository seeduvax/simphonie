/*
 * @file ExInvalidObjectType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/ExInvalidObjectType.hpp"
#include <sstream>

namespace simph {
	namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidObjectType::ExInvalidObjectType(Smp::IObject* sender,
                                Smp::IObject* invalid):
        _invalidObject(invalid) {
    setName("InvalidObjectType");
    setSender(sender);
    std::ostringstream d;
    d << "Object "<<invalid->GetName()<<" has invalid type.";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidObjectType::~ExInvalidObjectType() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::IObject* ExInvalidObjectType::GetInvalidObject() const noexcept {
    return _invalidObject;
}
}} // namespace simph::smpdk
