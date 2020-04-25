/*
 * @file ExCannotDelete.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/ExCannotDelete.hpp"
#include <sstream>

namespace simph {
	namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
ExCannotDelete::ExCannotDelete(Smp::IContainer* sender, Smp::IComponent* comp):
                _comp(comp) {
    _lower=sender->GetLower();
    setName("CannotDelete");
    setSender(sender);
    std::ostringstream d;
    d << "From "<<sender->GetName()<<": can't remove component "
      << comp->GetName() << ", min size "<<_lower
      << " is reached.";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExCannotDelete::~ExCannotDelete() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExCannotDelete::GetContainerName() const noexcept {
    return GetSender()->GetName();
}
// ..........................................................
const Smp::IComponent* ExCannotDelete::GetComponent() const noexcept {
    return _comp;
}
// ..........................................................
Smp::Int64 ExCannotDelete::GetLowerLimit() const noexcept {
    return _lower;
}


}} // namespace simph::smpdk
