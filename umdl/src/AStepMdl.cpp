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
using simph::sys::Callback;

// --------------------------------------------------------------------
// ..........................................................
AStepMdl::AStepMdl(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simph::kern::Component(name, descr, parent) {
    addEP("step", "Main model entry point", &AStepMdl::step, this);
}
// ..........................................................
AStepMdl::~AStepMdl() {}
// --------------------------------------------------------------------
}  // namespace umdl
}  // namespace simph
