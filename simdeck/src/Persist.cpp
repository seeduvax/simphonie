/*
 * @file Persist.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/Persist.hpp"

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
Persist::Persist(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent, Smp::Bool overrideNameCheck) : Object(name, descr, parent, overrideNameCheck) {}
// ..........................................................
Persist::~Persist() {}
// --------------------------------------------------------------------
// ..........................................................
void Persist::Restore(Smp::IStorageReader* reader) {
}
// ..........................................................
void Persist::Store(Smp::IStorageWriter* reader) {
}

}  // namespace simdeck
