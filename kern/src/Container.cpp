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
#include "simph/kern/ExDuplicateName.hpp"
#include "simph/kern/ExContainerFull.hpp"
#include "simph/kern/ExInvalidObjectType.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Container::Container(Smp::String8 name,Smp::String8 descr, 
                Smp::IObject* parent): Object(name,descr,parent),
                        _content("Content","",this) {
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
        throw ExContainerFull(this);
    }
    if (!checkComponentType(component)) {
        throw ExInvalidObjectType(this, component);
    }
    if (_content.at(component->GetName())!=nullptr) {
        throw ExDuplicateName(this,component->GetName());
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
