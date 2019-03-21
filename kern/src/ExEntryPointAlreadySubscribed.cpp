/*
 * @file ExEntryPointAlreadySubscribed.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExEntryPointAlreadySubscribed.hpp"
#include <sstream>

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExEntryPointAlreadySubscribed::ExEntryPointAlreadySubscribed(
                Smp::IObject* sender,
                const Smp::IEntryPoint* ep,
                Smp::String8 evName):
            _entryPoint(ep), _evName(evName) {
    setSender(sender);
    setName("EntryPointAlreadySubscribed");
    std::ostringstream d;
    d<<"Entry point "<<ep->GetName()<<" already subscribed to event"
        <<evName;
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExEntryPointAlreadySubscribed::~ExEntryPointAlreadySubscribed() {
}
// --------------------------------------------------------------------
// ..........................................................
const Smp::IEntryPoint* ExEntryPointAlreadySubscribed::GetEntryPoint() const noexcept {
    return _entryPoint;
}
// ..........................................................
Smp::String8 ExEntryPointAlreadySubscribed::GetEventName() const noexcept {
    return _evName;
}

}} // namespace simph::kern
