/*
 * @file ExDuplicateName.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/ExDuplicateName.hpp"
#include <sstream>

namespace simph {
	namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
ExDuplicateName::ExDuplicateName(Smp::IObject* sender, Smp::String8 dName) {
    _dName=dName;
    setName("DuplicateName");
    setSender(sender);
    std::ostringstream d;
    d << "From "<<sender->GetName() << ": object named "
            <<_dName<<" already exists.";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExDuplicateName::~ExDuplicateName() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExDuplicateName::GetDuplicateName() const noexcept {
    return _dName.c_str();
}

}} // namespace simph::smpdk
