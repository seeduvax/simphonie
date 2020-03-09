/*
 * @file ExInvalidArrayIndex.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExInvalidArrayIndex.hpp"
#include <sstream>

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidArrayIndex::ExInvalidArrayIndex(const Smp::IObject* sender,
                            Smp::Int64 invalid,
                            Smp::Int64 size):
                    _invalidSize(invalid),
                    _arraySize(size) {
    setName("InvalidArraySize");
    setSender(sender);
    std::ostringstream d;
    d << "Invalid array size "<<invalid
        <<" out of array bounds [0.."<<size<<"[.";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidArrayIndex::~ExInvalidArrayIndex() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::Int64 ExInvalidArrayIndex::GetInvalidIndex() const noexcept {
    return _invalidSize;
}
// ..........................................................
Smp::Int64 ExInvalidArrayIndex::GetArraySize() const noexcept {
    return _arraySize;
}

}} // namespace simph::kern
