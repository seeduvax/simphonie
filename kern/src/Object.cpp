/*
 * @file Object.cpp
 *
 * Copyright 2019 S.Devaux . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Object.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Object::Object(Smp::String8 name,
                Smp::String8 descr,
                Smp::IObject* parent) {
    _name=name;
    _description=descr;
    _parent=parent;
}
// ..........................................................
Object::~Object() {
}
// --------------------------------------------------------------------
// ..........................................................
bool Object::checkName() {
    // only alphanum chars, _, [, and ] are valid
    // in names.
    for (auto ch: _name) {
        if (!(
                (ch>='A' && ch<='Z')
	        || (ch>='a' && ch<='z')
	        || (ch>='0' && ch<='9')
	        || ch=='_'
	        || ch=='['
	        || ch==']'
	        )) {
	    return false;
	}
    }
    // empty names and C++ keywords are not valid.
    if ( _name=="" 
	|| _name=="for"
    ) {
        return false;
    }

    return true;
}
// ..........................................................
Smp::String8 Object::GetName() const {
    return _name.c_str();
}
// ..........................................................
Smp::String8 Object::GetDescription() const {
    return _description.c_str();
}
// ..........................................................
Smp::IObject* Object::GetParent() const {
    return _parent;
}
// ..........................................................
void Object::setName(Smp::String8 name) {
    auto nb=_name;
    _name=name;
    if (!checkName()) {
        _name=nb;
	// TODO log something + execption
    }
}
// ..........................................................
void Object::setDescription(Smp::String8 description) {
    _description=description;
}
// ..........................................................
void Object::setParent(Smp::IObject* parent) {
    _parent=parent;
}
// ..........................................................


}} // namespace simph::kern
