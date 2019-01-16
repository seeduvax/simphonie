/*
 * @file Composite.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Composite.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Composite::Composite() {
}
// ..........................................................
Composite::~Composite() {
}
// --------------------------------------------------------------------
// ..........................................................
const Smp::ContainerCollection* Composite::GetContainers() const {
    return &_containers;
}
// ..........................................................
Smp::IContainer* Composite::GetContainer(Smp::String8 name) const {
    return _containers.at(name);
}

}} // namespace simph::kern
