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

namespace {

bool checkName(const std::string& name) {
    //TODO use STL instead

    // only alphanum chars, _, [, and ] are valid
    // in names.
    for (auto ch: name) {
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
    if ( name==""
    || name=="for"
    ) {
        return false;
    }

    return true;
}
} // anonymous namespace

// --------------------------------------------------------------------
// ..........................................................
Object::Object(Smp::String8 name,
                Smp::String8 descr,
                Smp::IObject* parent):
                    _name(name)
                  , _description(descr)
                  , _parent(parent) {
    if (!checkName(_name)) {
        // TODO log something + exception
    }
}
// ..........................................................
Object::~Object() {
}
// --------------------------------------------------------------------
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
    _name=name;
    if (!checkName(_name)) {
    	// TODO log something + exception
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
