/*
 * @file ExNotContained.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/ExNotContained.hpp"
#include <sstream>

namespace simph {
	namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
ExNotContained::ExNotContained(Smp::IContainer* sender, Smp::IComponent* comp):
            _comp(comp) {
    setName("NotContained");
    setSender(sender);
    std::ostringstream d;
    d << "From "<<sender->GetName()<<": can't remove "<<comp->GetName()
        <<", not member.";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExNotContained::~ExNotContained() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExNotContained::GetContainerName() const noexcept {
    return GetSender()->GetName();
}
// ..........................................................
const Smp::IComponent* ExNotContained::GetComponent() const noexcept {
    return _comp;
}

}} // namespace simph::smpdk
