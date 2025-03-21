/*
 * @file Composite.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/Composite.hpp"
#include "simdeck/Container.hpp"

namespace simdeck {
#define TRACE(expr) std::cout << __FILE__ << ":" << __LINE__ << ": " << #expr " = " << expr << std::endl

AComposite::AComposite() : _containers("Containers", "", this) {}

Composite::Composite(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent) 
    : Object(name, descr, parent) {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::IContainer* AComposite::addContainer(Smp::String8 name, Smp::String8 descr) {
TRACE(name);
    auto c=new Container(name, descr, this);
    _containers.push_back(c);
    return c;
}
// --------------------------------------------------------------------
// ..........................................................
const Smp::ContainerCollection* AComposite::GetContainers() const {
    return &_containers;
}
// ..........................................................
Smp::IContainer* AComposite::GetContainer(Smp::String8 name) const {
TRACE(name);
    return _containers.at(name);
}
}  // namespace simdeck
