/*
 * @file Init.cpp
 *
 * Copyright 2019. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "Smp/ISimulator.h"
#include "simdeck/Factory.hpp"
#include "simphonie/kern/Logger.hpp"
#include "simphonie/kern/LoggerAsync.hpp"
#include "simphonie/kern/LoggerFile.hpp"
#include "simphonie/kern/LoggerOStream.hpp"
#include "simphonie/sys/DlDef.h"

namespace simphonie {
namespace kern {

REGISTER_SMP_LIBINIT();
ADD_SMP_FACTORY("Logger", Logger);
ADD_SMP_FACTORY("LoggerOStream", LoggerOStream);
ADD_SMP_FACTORY("LoggerAsync", LoggerAsync);
ADD_SMP_FACTORY("LoggerFile", LoggerFile);

} /* namespace kern */
}  // namespace simphonie
