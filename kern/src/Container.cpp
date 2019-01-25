/*
 * @file Container.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Container.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Container::Container() {
}
// ..........................................................
Container::~Container() {
}
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
    // default: any kond of component is allowed.
    return true;
}
// ..........................................................
void Container::AddComponent(Smp::IComponent* component) {
    Smp::Int64 maxSize=GetUpper();
    if (maxSize>=0 && _content.size()>=maxSize) {
        // TODO throw ContainerFull exception
    }
    if (!checkComponentType(component)) {
        // TODO throw Smp::InvalidObjectType exception
    }
    if (_content.at(component->GetName())!=nullptr) {
        // TODO throw Smp::DuplicateName Exception
    }
    _content.push_back(component);
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

}} // namespace simph::kern
