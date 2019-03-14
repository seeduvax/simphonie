/*
 * @file ExContainerFull.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExContainerFull.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExContainerFull::ExContainerFull(Smp::IContainer* sender) {
    _size=sender->GetCount();
    setName("ContainerFull");
    setSender(sender);
    setDescription("Max capacity reached.");
    setMessage();
}
// ..........................................................
ExContainerFull::~ExContainerFull() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExContainerFull::GetContainerName() const noexcept {
    return GetSender()->GetName();
}
// ..........................................................
Smp::Int64 ExContainerFull::GetContainerSize() const noexcept {
    return _size;
}

}} // namespace simph::kern
