/*
 * @file AStepMdl.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "Smp/ISimulator.h"
#include "simdeck/Factory.hpp"
#include "simphonie/rest/RestService.hpp"
#include "simphonie/sys/DlDef.h"

namespace simphonie {
namespace rest {

REGISTER_SMP_LIBINIT();
ADD_SMP_FACTORY("RestService", RestService);
}  // namespace rest
}  // namespace simphonie
