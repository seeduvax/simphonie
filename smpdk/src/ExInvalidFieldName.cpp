/*
 * @file ExInvalidFieldName.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/ExInvalidFieldName.hpp"
#include <sstream>

namespace simph {
namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidFieldName::ExInvalidFieldName(const Smp::IObject* sender, Smp::String8 invalidName)  : _invalidName(invalidName) {
    setName("InvalidFieldName");
    setSender(sender);
    std::ostringstream d;
    d << "'" << _invalidName << "' does not refere to a field.";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidFieldName::~ExInvalidFieldName() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExInvalidFieldName::GetFieldName() const noexcept {
    return _invalidName.c_str();
}

}} // namespace simph::smpdk
