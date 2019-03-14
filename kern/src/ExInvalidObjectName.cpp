/*
 * @file ExInvalidObjectName.cpp
 *
 * Copyright 2019 . All rights reserved.
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
ExInvalidObjectName::ExInvalidObjectName(Smp::IObject* sender,
                                Smp::String8 invalidName):
                        _invalidName(invalidName) {
    setName("InvalidObjectName");
    setSender(sender);
    std::ostringstream d;
    d << "'" << _invalidName 
        << "' include char(s) not useable in a Smp object name.";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidObjectName::~ExInvalidObjectName() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExInvalidObjectName::GetInvalidName() const noexcept {
    return _invalidName;
}
}} // namespace simph::kern
