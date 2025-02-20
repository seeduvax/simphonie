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
#include "simphonie/sys/DlDef.h"
#include "simphonie/umdl/Logger1D.hpp"
#include "simphonie/umdl/SimTimeProvider.hpp"
#include "simphonie/umdl/SmpIncrement.hpp"
#include "simphonie/umdl/SysTimeSynchro.hpp"

namespace simphonie {
namespace umdl {

REGISTER_SMP_LIBINIT();
ADD_SMP_FACTORY("SimTimeProvider", SimTimeProvider);
ADD_SMP_FACTORY("SysTimeSynchro", SysTimeSynchro);
ADD_SMP_FACTORY("Logger1D", Logger1D);
ADD_SMP_FACTORY("SmpIncrement", SmpIncrement);

}  // namespace umdl
}  // namespace simph
