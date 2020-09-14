/*
 * @file ExInvalidFieldType.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExInvalidFieldType.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidFieldType::ExInvalidFieldType(Smp::IObject* sender, 
                                        Smp::String8 descr) {
    setSender(sender);
    setName("InvalidFieldType");
    setDescription(descr);
    setMessage();
}
// ..........................................................
ExInvalidFieldType::~ExInvalidFieldType() {
}

}} // namespace simph::kern
