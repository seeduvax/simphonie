/*
 * @file FallibleModel.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/umdl/FallibleModel.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
namespace umdl {
// --------------------------------------------------------------------
// ..........................................................
FallibleModel::FallibleModel(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent,
                             Smp::FailureCollection* failureList)
    : AStepMdl(name, descr, parent), _failureList(failureList) {}
// ..........................................................
FallibleModel::~FallibleModel() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::Bool FallibleModel::IsFailed() const {
    Smp::Bool failed = false;
    for (auto itr : *_failureList)
    // for(auto itr = _failureList->begin(); itr != _failureList->end(); ++itr)
    {
        if (itr->IsFailed() == true) {
            failed = true;
            break;
        }
    }
    return failed;
}
// ..........................................................
const Smp::FailureCollection* FallibleModel::GetFailures() const {
    return _failureList;
}
// ..........................................................
Smp::IFailure* FallibleModel::GetFailure(Smp::String8 name) const {
    Smp::IFailure* failure = nullptr;
    for (auto itr : *_failureList)
    // for(auto itr = _failureList->begin(); itr != _failureList->end(); ++itr)
    {
        if (itr->GetName() == name) {
            failure = itr;
            break;
        }
    }
    return failure;
}

}  // namespace umdl
}  // namespace simph
