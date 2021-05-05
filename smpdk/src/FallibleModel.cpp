/*
 * @file FallibleModel.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/FallibleModel.hpp"

namespace simph {
namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
FallibleModel::FallibleModel() : _failureList("failure", "", this) {}
// ..........................................................
FallibleModel::~FallibleModel() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::Bool FallibleModel::IsFailed() const {
    bool isFailed = false;
    for (auto itr : _failureList) {
        if (itr->IsFailed()) {
            isFailed = true;
            break;
        }
    }
    return isFailed;
}
// ..........................................................
void FallibleModel::AddFailure(Smp::IFailure* failure) {
    _failureList.push_back(failure);
}
// ..........................................................
const Smp::FailureCollection* FallibleModel::GetFailures() const {
    return &_failureList;
}
// ..........................................................
Smp::IFailure* FallibleModel::GetFailure(Smp::String8 name) const {
    Smp::IFailure* failure = nullptr;
    for (auto itr : _failureList) {
        if (std::string(itr->GetName()) == std::string(name)) {
            failure = itr;
            break;
        }
    }
    return failure;
}

}  // namespace smpdk
}  // namespace simph
