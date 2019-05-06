/*
 * @file ExTypeAlreadyRegistered.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExTypeAlreadyRegistered.hpp"
#include "Smp/Publication/IType.h"
#include <sstream>

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExTypeAlreadyRegistered::ExTypeAlreadyRegistered(Smp::IObject* sender,
                    Smp::String8 name, Smp::Publication::IType* type):
                    _name(name),
                    _type(type) {
    setName("TypeAlreadyRegistered");
    setSender(sender);
    std::ostringstream d;
    d << "Can't register type " << name 
        <<", type "<< type->GetName()<<" with same uuid "
        /* TODO <<type->GetUuid()*/<<" already registered.";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExTypeAlreadyRegistered::~ExTypeAlreadyRegistered() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExTypeAlreadyRegistered::GetTypeName() const noexcept {
    return _name;
}
// ..........................................................
Smp::Publication::IType* ExTypeAlreadyRegistered::GetType() const noexcept {
    return _type;
}
}} // namespace simph::kern
