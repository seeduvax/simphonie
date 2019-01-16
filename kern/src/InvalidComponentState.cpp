/*
 * @file InvalidComponentState.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/InvalidComponentState.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
InvalidComponentState::InvalidComponentState(Smp::ComponentStateKind invalid,
            Smp::ComponentStateKind expected): 
                _invalid(invalid),_expected(expected) {
}
// ..........................................................
InvalidComponentState::~InvalidComponentState() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::ComponentStateKind InvalidComponentState::GetInvalidState() const noexcept {
    return _invalid;
}
// ..........................................................
Smp::ComponentStateKind InvalidComponentState::GetExpectedState() const noexcept {
    return _expected;
}

}} // namespace simph::kern
