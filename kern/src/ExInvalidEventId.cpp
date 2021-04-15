/*
 * @file ExInvalidEventId.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExInvalidEventId.hpp"
#include <sstream>

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidEventId::ExInvalidEventId(Smp::IObject* sender, Smp::Services::EventId invalidId) : _invalidId(invalidId) {
    setSender(sender);
    setName("IvalideventId");
    std::ostringstream d;
    d << "event id " << invalidId << " is not valid.";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidEventId::~ExInvalidEventId() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::Services::EventId ExInvalidEventId::GetInvalidEventId() const noexcept {
    return _invalidId;
}

}  // namespace kern
}  // namespace simph
