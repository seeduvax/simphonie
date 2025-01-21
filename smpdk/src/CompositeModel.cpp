/*
 * @file CompositeModel.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/CompositeModel.hpp"

namespace simph {
namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
CompositeModel::CompositeModel(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
                Model(name, descr, parent) {
}
// ..........................................................
CompositeModel::~CompositeModel() {
}

}} // namespace simph::smpdk
