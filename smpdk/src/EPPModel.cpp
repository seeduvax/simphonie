/*
 * @file EPPModel.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/EPPModel.hpp"

namespace simph {
namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
EPPModel::EPPModel(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
            Parent(name, descr, parent) {
}
// ..........................................................
EPPModel::~EPPModel() {
}

}} // namespace simph::smpdk
