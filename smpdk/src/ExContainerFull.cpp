/*
 * @file ExContainerFull.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/ExContainerFull.hpp"
#include <sstream>

namespace simph {
namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
ExContainerFull::ExContainerFull(Smp::IContainer* sender) {
    _size = sender->GetCount();
    setName("ContainerFull");
    setSender(sender);
    std::ostringstream d;
    d << "From " << sender->GetName() << ": max size " << sender->GetUpper() << " is reached, can't add more content.";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExContainerFull::~ExContainerFull() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExContainerFull::GetContainerName() const noexcept {
    return GetSender()->GetName();
}
// ..........................................................
Smp::Int64 ExContainerFull::GetContainerSize() const noexcept {
    return _size;
}

}  // namespace smpdk
}  // namespace simph
