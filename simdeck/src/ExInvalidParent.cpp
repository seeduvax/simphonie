/*
 * @file ExInvalidParent.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/ExInvalidParent.hpp"
#include "Smp/IObject.h"
#include <sstream>

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidParent::ExInvalidParent(const Smp::IObject* sender, const Smp::IObject* found, Smp::IObject* expected):
            _found(found),
            _expected(expected) {
    setName("InvalidParent");
    setSender(sender);
    std::ostringstream d;
    d << "Invalid parent to add chidl to "<<sender->GetName()
      <<" found: "
      <<(found!=nullptr?found->GetName():"nullptr")
      <<", expected:" 
      << (expected!=nullptr?expected->GetName():"nullptr") <<".";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidParent::~ExInvalidParent() {
}
// --------------------------------------------------------------------
// ..........................................................
const Smp::IObject* ExInvalidParent::GetParentFound() const noexcept {
    return _found;
}
// ..........................................................
Smp::IObject* ExInvalidParent::GetParentExpected() noexcept {
    return _expected;
}


} // namespace simdeck
