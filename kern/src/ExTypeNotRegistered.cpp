/*
 * @file ExTypeNotRegistered.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExTypeNotRegistered.hpp"
#include <sstream>

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExTypeNotRegistered::ExTypeNotRegistered(Smp::IObject* sender, Smp::Uuid typeUuid) : _uuid(typeUuid) {
    setName("TypeNotRegistered");
    setSender(sender);
    std::ostringstream d;
    d << "Not type registered with uuid " << typeUuid;
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExTypeNotRegistered::~ExTypeNotRegistered() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::Uuid ExTypeNotRegistered::GetUuid() const noexcept {
    return _uuid;
}

}  // namespace kern
}  // namespace simph
