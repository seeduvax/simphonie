/*
 * @file ExInvalidSimulationTime.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExInvalidSimulationTime.hpp"
#include <sstream>

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidSimulationTime::ExInvalidSimulationTime(Smp::IObject* sender,
            Smp::Duration cTime, Smp::Duration pTime, Smp::Duration mTime):
            _cTime(cTime), _pTime(pTime), _mTime(mTime) {
    setSender(sender);
    setName("InvalidSimulationTime");
    std::ostringstream d;
    d << "provided time "<<_pTime<<" not bounded in [ "
        <<_cTime<<" ; "<<_mTime<<" ]";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidSimulationTime::~ExInvalidSimulationTime() {
}

// --------------------------------------------------------------------
// ..........................................................
Smp::Duration ExInvalidSimulationTime::GetCurrentTime() const noexcept {
    return _cTime;
}
// ..........................................................
Smp::Duration ExInvalidSimulationTime::GetProvidedTime() const noexcept {
    return _pTime;
}
// ..........................................................
Smp::Duration ExInvalidSimulationTime::GetMaximumTime() const noexcept {
    return _mTime;
}
}} // namespace simph::kern
