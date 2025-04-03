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
#include "simphonie/colibry/FieldRecorder.hpp"
#include "simphonie/colibry/FieldRecorderCsv.hpp"

namespace simphonie {
namespace colibry {

REGISTER_SMP_LIBINIT();
ADD_SMP_FACTORY("FieldRecorderCsv", FieldRecorderCsv);

}  // namespace colibry
}  // namespace simph
