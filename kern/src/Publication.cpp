/*
 * @file Publication.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Publication.hpp"
#include "simph/smpdk/ExDuplicateName.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Publication::Publication(Smp::IObject* toPublish, Smp::IObject* parent):
            _pubObj(toPublish),
            _parent(parent) {
}
// ..........................................................
Publication::~Publication() {
    for (auto ch: _childs) {
        delete ch;
    }
    delete _pubObj;
}
// --------------------------------------------------------------------
// IObject implementation, mostly binding to the published object
// ..........................................................
Smp::String8 Publication::GetName() const {
    return _pubObj->GetName();
}
// ..........................................................
Smp::String8 Publication::GetDescription() const {
    return _pubObj->GetDescription();
}
// ..........................................................
Smp::IObject* Publication::GetParent() const {
    return _parent;
}
// --------------------------------------------------------------------
// Childs management
// ..........................................................
void Publication::addChild(Publication* pub) throw (Smp::DuplicateName) {
    Publication* p=getChild(pub->GetName());
    if (p!=nullptr) {
        throw simph::smpdk::ExDuplicateName(_pubObj,pub->GetName());
    }
    _childs.push_back(pub);
}
// ..........................................................
Publication* Publication::getChild(Smp::String8 name) const {
    for (auto ch: _childs) {
        if (strcmp(ch->GetName(),name)==0) {
            return ch; 
        }
    }
    return nullptr;
}

// ..........................................................


}} // namespace simph::kern
