/*
 * @file ExInvalidTarget.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExInvalidTarget.hpp"
#include "Smp/IDataflowField.h"
#include <sstream>

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidTarget::ExInvalidTarget(Smp::IDataflowField* source,
                                Smp::IField* target):
                _source(source), _target(target) {
    setName("InvalidTarget");                
    setSender(source);
    std::ostringstream d;
    d << "Flow field "<<source->GetName() 
        << " is not compatible with target "
        << target->GetName();
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidTarget::~ExInvalidTarget() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::IDataflowField* ExInvalidTarget::GetSource() const noexcept {
    return _source;
}
// ..........................................................
Smp::IField* ExInvalidTarget::GetTarget() const noexcept {
    return _target;
}

}} // namespace simph::kern
