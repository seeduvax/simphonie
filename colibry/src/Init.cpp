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
#include "simphonie/colibry/FieldRecorder.hpp"
#include "simphonie/colibry/FieldRecorderCsv.hpp"
#include "simphonie/colibry/FieldRecorderHDF5.hpp"
#include "simphonie/colibry/SchedulerTracker.hpp"
#include "simphonie/colibry/SimControl.hpp"
#include "simphonie/colibry/Synchronizer.hpp"
#include "simphonie/sys/DlDef.h"

namespace simphonie {
namespace colibry {

REGISTER_SMP_LIBINIT();
ADD_SMP_FACTORY("FieldRecorderCsv", FieldRecorderCsv);
ADD_SMP_FACTORY("FieldRecorderHDF5", FieldRecorderHDF5);
ADD_SMP_FACTORY("SimControl", SimControl);
ADD_SMP_FACTORY("SchedulerTracker", SchedulerTracker);
ADD_SMP_FACTORY("Synchronizer", Synchronizer);

}  // namespace colibry
}  // namespace simph
