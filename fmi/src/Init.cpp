/*
 * @file Init.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "Smp/ISimulator.h"
#include "simdeck/Factory.hpp"
#include "simphonie/fmi/FMILoad.hpp"
#include "simphonie/sys/DlDef.h"

namespace simphonie {
namespace fmi {

REGISTER_SMP_LIBINIT();
ADD_SMP_FACTORY("FMILoad", FMILoad);

} /* namespace fmi */
} /* namespace simphonie */
