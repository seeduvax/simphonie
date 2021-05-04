/*
 * @file ExInvalidObjectName.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExInvalidObjectName.hpp"
#include <sstream>

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidObjectName::ExInvalidObjectName(Smp::IObject* sender, Smp::String8 objname) : _objname(objname) {
    setName("InvalidObjectName");
    setSender(sender);
    std::ostringstream d;
    d << "Object name \"" << objname << "\" is not valid ";
    setDescription(d.str().c_str());
    setMessage("");
}
// ..........................................................
ExInvalidObjectName::~ExInvalidObjectName() {}

Smp::String8 ExInvalidObjectName::GetInvalidName() const noexcept {
    return _objname;
}

}  // namespace kern
}  // namespace simph
