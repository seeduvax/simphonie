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
#include "simph/svc/Sampler.hpp"
#include "simph/sys/DlDef.h"

namespace simph {
namespace svc {

REGISTER_SMP_LIBINIT();
ADD_SMP_FACTORY("Sampler", Sampler);

}  // namespace svc
}  // namespace simph
