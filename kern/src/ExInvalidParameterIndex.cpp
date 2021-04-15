/*
 * @file ExInvalidParameterIndex.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExInvalidParameterIndex.hpp"
#include <sstream>

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidParameterIndex::ExInvalidParameterIndex(Smp::IObject* sender, const Smp::String8 opName, Smp::Int32 index,
                                                 Smp::Int32 count)
    : _opName(opName), _index(index), _count(count) {
    setName("InvalidParameterIndex");
    setSender(sender);
    std::ostringstream d;
    d << "Can't find parameter #" << index << " for operation " << opName << ". Operation parameter count is " << count;
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidParameterIndex::~ExInvalidParameterIndex() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExInvalidParameterIndex::GetOperationName() const noexcept {
    return _opName;
}
// ..........................................................
Smp::Int32 ExInvalidParameterIndex::GetParameterIndex() const noexcept {
    return _index;
}
// ..........................................................
Smp::Int32 ExInvalidParameterIndex::GetParameterCount() const noexcept {
    return _count;
}
// ..........................................................

}  // namespace kern
}  // namespace simph
