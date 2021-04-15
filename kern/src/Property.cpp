/*
 * @file Property.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Property.hpp"

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Property::Property(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent, Smp::Publication::IType* type,
                   Smp::AccessKind access, Smp::ViewKind view)
    : simph::smpdk::Object(name, descr, parent), _type(type), _access(access), _view(view) {}
// ..........................................................
Property::~Property() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::Publication::IType* Property::GetType() const {
    return _type;
}
// ..........................................................
Smp::AccessKind Property::GetAccess() const {
    return _access;
}
// ..........................................................
Smp::ViewKind Property::GetView() const {
    return _view;
}
// ..........................................................
Smp::AnySimple Property::GetValue() const {
    return _value;
}
// ..........................................................
void Property::SetValue(Smp::AnySimple value) {
    _value = value;
}

}  // namespace kern
}  // namespace simph
