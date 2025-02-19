/*
 * @file Model.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/Model.hpp"

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
Model::Model(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
            Parent(name, descr, parent) {
}
// ..........................................................
Model::~Model() {
}

} // namespace simdeck
