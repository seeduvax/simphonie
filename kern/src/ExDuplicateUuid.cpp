/*
 * @file ExDuplicateUuid.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExDuplicateUuid.hpp"
#include <sstream>

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExDuplicateUuid::ExDuplicateUuid(Smp::IObject* sender, Smp::String8 oldName,
                                        Smp::String8 newName):
                    _oldName(oldName),
                    _newName(newName) {
    setName("DuplicateUuid");
    setSender(sender);
    std::ostringstream d;
    d << "Object named "<<oldName
        <<" already registered same uuid than object "
        <<newName;
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExDuplicateUuid::~ExDuplicateUuid() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExDuplicateUuid::GetOldName() const noexcept {
    return _oldName;
}
// ..........................................................
Smp::String8 ExDuplicateUuid::GetNewName() const noexcept {
    return _newName;
}

}} // namespace simph::kern
