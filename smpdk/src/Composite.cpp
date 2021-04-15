/*
 * @file Composite.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/Composite.hpp"
#include "simph/smpdk/Container.hpp"

namespace simph {
namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
Composite::Composite(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Object(name, descr, parent), _containers("Containers", "", this) {}
// ..........................................................
Composite::~Composite() {}
// --------------------------------------------------------------------
// ..........................................................
void Composite::addContainer(Smp::String8 name, Smp::String8 descr) {
    _containers.push_back(new Container(name, descr, this));
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
}  // namespace smpdk
}  // namespace simph
