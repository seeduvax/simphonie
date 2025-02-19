/*
 * @file Failure.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/Failure.hpp"

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
Failure::Failure(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Persist(name, descr, parent), _failed(false) {}
// ..........................................................
Failure::~Failure() {}
// --------------------------------------------------------------------
// ..........................................................
void Failure::Fail() {
    if (_failed == true) {
// TODO get logger from parent ?        LOGW(this->GetName() << " is already faulty");
    }
    else {
        // TODO
        doFail();
        _failed = true;
    }
}
// ..........................................................
void Failure::doFail() {
}
// ..........................................................
void Failure::Unfail() {
    if (_failed == false) {
// TODO get logger from parent ?        LOGW(this->GetName() << " isn't faulty");
    }
    else {
        // TODO
        doUnfail();
        _failed = false;
    }
}
// ..........................................................
void Failure::doUnfail() {
}

Smp::Bool Failure::IsFailed() const {
    return _failed;
}

}  // namespace simdeck
