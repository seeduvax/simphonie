/*
 * @file ClassType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ClassType.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ClassType::ClassType(Smp::Uuid uuid, Smp::String8 name, 
            Smp::String8 description, Smp::IObject* parent):
        StructureType(uuid,name,description,parent) {
}
// ..........................................................
ClassType::~ClassType() {
}

}} // namespace simph::kern
