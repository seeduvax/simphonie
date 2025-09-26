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
#include "SmpIncrementC.h"
#include "simdeck/Factory.hpp"
#include "simphonie/sys/DlDef.h"
#include "simphonie/umdl/Logger1D.hpp"
#include "simphonie/umdl/SimTimeProvider.hpp"
#include "simphonie/umdl/SmpIncrement.hpp"
#include "simphonie/umdl/SysTimeSynchro.hpp"

/* TODO in simdeck/Factory.hpp */
/* Pour eviter des erreurs à la compile puisque simdeck pas à jour
#include "simdeck/C/C2SMPBridge.hpp"
#define ADD_SMP_FACTORY_C(name, initFunction) \
    static int static_add_##initFunction{[] {                                                                          \
        _createFactoryFns.push_back(                                                                                   \
            [&](Smp::ISimulator* simulator) { return new
simdeck::Factory<simphonie::kern::C2SMPBridge<&initFunction>>(name, name, simulator); }); \
        return 1;                                                                                                      \
    }()};
*/

namespace simphonie {
namespace umdl {

REGISTER_SMP_LIBINIT();
ADD_SMP_FACTORY("SimTimeProvider", SimTimeProvider);
ADD_SMP_FACTORY("SysTimeSynchro", SysTimeSynchro);
ADD_SMP_FACTORY("Logger1D", Logger1D);
ADD_SMP_FACTORY("SmpIncrement", SmpIncrement);
/* ADD_SMP_FACTORY_C("SmpIncrementC", SmpIncrementC); */

}  // namespace umdl
}  // namespace simph
