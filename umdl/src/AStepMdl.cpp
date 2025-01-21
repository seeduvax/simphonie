/*
 * @file AStepMdl.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/umdl/AStepMdl.hpp"
#include "simph/sys/Callback.hpp"

namespace simph {
namespace umdl {

// --------------------------------------------------------------------
// ..........................................................
AStepMdl::AStepMdl(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Parent(name, descr, parent) {
    addEP("step", "Main model entry point", this, &AStepMdl::step);
}
// ..........................................................
AStepMdl::~AStepMdl() {}
// --------------------------------------------------------------------
}  // namespace umdl
}  // namespace simph
