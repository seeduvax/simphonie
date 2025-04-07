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
#include "simphonie/esmp/Sampler.hpp"
#include "simphonie/sys/DlDef.h"

namespace simphonie {
namespace esmp {

REGISTER_SMP_LIBINIT();
ADD_SMP_FACTORY("Sampler", Sampler);

}  // namespace esmp
}  // namespace simph
