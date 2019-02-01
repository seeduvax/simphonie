/*
 * @file Persist.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Persist.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Persist::Persist() {
}
// ..........................................................
Persist::~Persist() {
}
// --------------------------------------------------------------------
// ..........................................................
void Persist::Restore(Smp::IStorageReader* reader) {
LOGE("simph::kern::Persist::Restore not implemented yet!!!!")
}
// ..........................................................
void Persist::Store(Smp::IStorageWriter* reader) {
LOGE("simph::kern::Persist::Store not implemented yet!!!!")
}

}} // namespace simph::kern
