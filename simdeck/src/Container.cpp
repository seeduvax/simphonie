/*
 * @file Container.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/Container.hpp"
#include "simdeck/ExCannotDelete.hpp"
#include "simdeck/ExContainerFull.hpp"
#include "simdeck/ExDuplicateName.hpp"
#include "simdeck/ExInvalidObjectType.hpp"
#include "simdeck/ExInvalidParent.hpp"
#include "simdeck/ExNotContained.hpp"

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
Container::Container(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Object(name, descr, parent), _content("Content", "", this) {}
// ..........................................................
Container::~Container() {}
// --------------------------------------------------------------------
// ..........................................................
const Smp::ComponentCollection* Container::GetComponents() const {
    return &_content;
}
// ..........................................................
Smp::IComponent* Container::GetComponent(Smp::String8 name) const {
    return _content.at(name);
}
// ..........................................................
bool Container::checkComponentType(Smp::IComponent* component) {
    // default: any kind of component is allowed.
    return true;
}
// ..........................................................
void Container::AddComponent(Smp::IComponent* component) {
    if (component->GetParent()!=GetParent()) {
        throw ExInvalidParent(this,component->GetParent(),GetParent());
    }
    Smp::Int64 maxSize = GetUpper();
    if (maxSize >= 0 && _content.size() >= maxSize) {
        throw ExContainerFull(this);
    }
    if (!checkComponentType(component)) {
        throw ExInvalidObjectType(this, component);
    }
    if (_content.at(component->GetName()) != nullptr ||
            (GetParent()!=nullptr && component->GetParent()==GetParent()
             && GetParent()->GetChild(component->GetName())!=nullptr)) {
        throw ExDuplicateName(this, component->GetName());
    }
    _content.push_back(component);
}
// ..........................................................
void Container::DeleteComponent(Smp::IComponent* component) {
    if (_content.size() <= GetLower()) {
        throw ExCannotDelete(this, component);
    }
    if (_content.remove(component)) {
        delete component;
    }
    else {
        throw ExNotContained(this, component);
    }
}
// ..........................................................
Smp::Int64 Container::GetCount() const {
    return _content.size();
}
// ..........................................................
Smp::Int64 Container::GetUpper() const {
    return -1;
}
// ..........................................................
Smp::Int64 Container::GetLower() const {
    return 0;
}

}  // namespace simdeck
