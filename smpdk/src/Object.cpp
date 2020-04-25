/*
 * @file Object.cpp
 *
 * Copyright 2019 S.Devaux . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/Object.hpp"
#include "simph/smpdk/ExInvalidObjectName.hpp"
#include <regex>

namespace simph {
    namespace smpdk {

// --------------------------------------------------------------------
// ..........................................................
Object::Object(Smp::String8 name,
                Smp::String8 descr,
                Smp::IObject* parent):
                    _name(name)
                  , _description(descr)
                  , _parent(parent) {
    if (!checkName(name)) {
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
bool Object::checkName(Smp::String8 name) {
    std::string n=name;
    // Check for non empty alphanumeric names.
    // '_', '[' and ']' are also valid in names
    if (!std::regex_match(n,std::regex("[a-zA-Z][a-zA-Z0-9_\\[\\]]*"))) {
        return false;
    }
    // C++ keywords are not valid.
    // TODO S.Devaux: about C++ keywords I read this somewhere but is is not
    // in the Smp::IObject header. Is it really an issue to have an object
    // named 'while'?
    static const std::vector<std::string> forbidden = {
        "for","while","do","if","else","return","public","protected","private"
    };
    for (auto kw: forbidden) {
        if (kw==n) {
            return false;
        }
    }
    return true;
}


}} // namespace simph::smpdk
