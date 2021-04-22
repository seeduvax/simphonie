/*
 * @file Failure.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Failure.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
namespace kern {
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
        LOGW(this->GetName() << " is already faulty");
    }
    else {
        // TODO
        doFail();
    }
}
// ..........................................................
void Failure::doFail() {
    _failed = true;
}
// ..........................................................
void Failure::Unfail() {
    if (_failed == false) {
        LOGW(this->GetName() << " isn't faulty");
    }
    else {
        // TODO
        doUnfail();
    }
}
// ..........................................................
void Failure::doUnfail() {
    _failed = false;
}

Smp::Bool Failure::IsFailed() const {
    return _failed;
}

}  // namespace kern
}  // namespace simph
