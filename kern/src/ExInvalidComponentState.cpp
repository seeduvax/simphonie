/*
 * @file ExInvalidComponentState.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <sstream>
#include "simph/kern/ExInvalidComponentState.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidComponentState::ExInvalidComponentState(Smp::IObject* sender,
            Smp::ComponentStateKind invalid,
            Smp::ComponentStateKind expected): 
                _invalid(invalid),_expected(expected) {
    setName("InvalidComponentState");
    setSender(sender);
    std::ostringstream d;
    d<<"current state is "<<invalid<<" while expected is "<<expected<<"."<<std::ends;;
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidComponentState::~ExInvalidComponentState() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::ComponentStateKind ExInvalidComponentState::GetInvalidState() const noexcept {
    return _invalid;
}
// ..........................................................
Smp::ComponentStateKind ExInvalidComponentState::GetExpectedState() const noexcept {
    return _expected;
}

}} // namespace simph::kern
