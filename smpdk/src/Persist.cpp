/*
 * @file Persist.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/Persist.hpp"

namespace simph {
namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
Persist::Persist(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent) : Object(name, descr, parent) {}
// ..........................................................
Persist::~Persist() {}
// --------------------------------------------------------------------
// ..........................................................
void Persist::Restore(Smp::IStorageReader* reader) {
// TODO    LOGE("simph::smpdk::Persist::Restore not implemented yet!!!!")
}
// ..........................................................
void Persist::Store(Smp::IStorageWriter* reader) {
// TODO    LOGE("simph::smpdk::Persist::Store not implemented yet!!!!")
}

}  // namespace smpdk
}  // namespace simph
