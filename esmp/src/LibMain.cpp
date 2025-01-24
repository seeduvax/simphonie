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
#include "simph/smpdk/Factory.hpp"
#include "simph/esmp/Sampler.hpp"
#include "simph/sys/DlDef.h"

namespace simph {
namespace esmp {

REGISTER_SMP_LIBINIT();
ADD_SMP_FACTORY("Sampler", Sampler);

}  // namespace esmp
}  // namespace simph
