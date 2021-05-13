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
#include "simph/kern/Factory.hpp"
#include "simph/sys/DlDef.h"
#include "simph/umdl/Logger1D.hpp"
#include "simph/umdl/SimTimeProvider.hpp"
#include "simph/umdl/SmpIncrement.hpp"
#include "simph/umdl/SysTimeSynchro.hpp"

namespace simph {
namespace umdl {

REGISTER_SMP_LIBINIT();
ADD_SMP_FACTORY("SimTimeProvider", SimTimeProvider);
ADD_SMP_FACTORY("SysTimeSynchro", SysTimeSynchro);
ADD_SMP_FACTORY("Logger1D", Logger1D);
ADD_SMP_FACTORY("SmpIncrement", SmpIncrement);

}  // namespace umdl
}  // namespace simph
