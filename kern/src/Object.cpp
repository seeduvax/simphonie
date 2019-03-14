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
#include "simph/kern/ExInvalidObjectName.hpp"

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
    // TODO S.Devaux: about C++ keywords I read this somewhere but is is not
    // in the Smp::IObject header. Is it really an issue to have an object
    // named 'while'?
    if ( name==""
        || name=="for"
        || name=="while"
        || name=="do"
        || name=="if"
        || name=="else"
        || name=="return"
        || name=="public"
        || name=="protected"
        || name=="private"
    ) {
        return false;
    }
    // finally check 1st is a letter.
    char c1=name[0];
    return (c1>='A' && c1<='Z') || (c1>='a' && c1<='z');
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
        throw ExInvalidObjectName(this,name);
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
    if (!checkName(name)) {
        throw ExInvalidObjectName(this,name);
    }
    _name=name;
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
