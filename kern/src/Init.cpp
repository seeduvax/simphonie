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
#include "simph/kern/Sampler.hpp"
#include "simph/sys/DlDef.h"

namespace simph {
namespace kern {

REGISTER_SMP_LIBINIT();
ADD_SMP_FACTORY("Sampler", Sampler);

}  // namespace kern
}  // namespace simph
