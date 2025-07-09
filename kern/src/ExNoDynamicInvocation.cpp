/*
 * @file ExNoDynamicInvocation.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/ExNoDynamicInvocation.hpp"
#include "Smp/IComponent.h"
#include <sstream>

namespace simphonie {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExNoDynamicInvocation::ExNoDynamicInvocation(const Smp::IComponent* sender): _component(sender) {
    setName("NoDynamicInvocation");
    setSender(sender);
    std::ostringstream d;
    d << "Object named " << sender->GetName() << "is not a DynamicInvocation (c'ant host properties and operations)";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExNoDynamicInvocation::~ExNoDynamicInvocation() {
}

// --------------------------------------------------------------------
// ..........................................................
const Smp::IComponent* ExNoDynamicInvocation::GetComponent() const noexcept {
    return _component;
}
}} // namespace simphonie::kern
