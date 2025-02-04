/*
 * @file ExInvalidObjectName.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/ExInvalidObjectName.hpp"
#include <sstream>

namespace simph {
namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidObjectName::ExInvalidObjectName(Smp::IObject* sender, Smp::String8 invalidName) : _invalidName(invalidName) {
    setName("InvalidObjectName");
    setSender(sender);
    std::ostringstream d;
    d << "'" << _invalidName << "' string is invalid as a SMP object name.";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidObjectName::~ExInvalidObjectName() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExInvalidObjectName::GetInvalidName() const noexcept {
    return _invalidName.c_str();
}
}  // namespace smpdk
}  // namespace simph
