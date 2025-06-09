/*
 * @file Init.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/Factory.hpp"
#include "simphonie/sys/DlDef.h"
#include "simphonie/mt/SimSyncMaster.hpp"
#include "simphonie/mt/SimSyncSlave.hpp"

namespace simphonie {
namespace mt {

REGISTER_SMP_LIBINIT();
ADD_SMP_FACTORY("SimSyncMaster", SimSyncMaster);
ADD_SMP_FACTORY("SimSyncSlave", SimSyncSlave);

}  /* namespace mt */
}  /* namespace simphonie */
