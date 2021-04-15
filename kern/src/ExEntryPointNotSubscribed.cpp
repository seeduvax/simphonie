/*
 * @file ExEntryPointNotSubscribed.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExEntryPointNotSubscribed.hpp"
#include <sstream>

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExEntryPointNotSubscribed::ExEntryPointNotSubscribed(Smp::IObject* sender, const Smp::IEntryPoint* ep,
                                                     Smp::String8 evName)
    : _entryPoint(ep), _evName(evName) {
    setSender(sender);
    setName("EntriyPointNotSubscribed");
    std::ostringstream d;
    d << "Entry point " << ep->GetName() << " not subscribed to event " << evName;
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExEntryPointNotSubscribed::~ExEntryPointNotSubscribed() {}
// --------------------------------------------------------------------
// ..........................................................
const Smp::IEntryPoint* ExEntryPointNotSubscribed::GetEntryPoint() const noexcept {
    return _entryPoint;
}
// ..........................................................
Smp::String8 ExEntryPointNotSubscribed::GetEventName() const noexcept {
    return _evName;
}
}  // namespace kern
}  // namespace simph
