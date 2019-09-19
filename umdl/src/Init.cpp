/*
 * @file AStepMdl.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/umdl/SimTimeProvider.hpp"
#include "simph/umdl/SysTimeSynchro.hpp"
#include "simph/umdl/Logger1D.hpp"
#include "simph/kern/Factory.hpp"
#include "Smp/ISimulator.h"
#include "simph/sys/DlDef.h"

extern "C" {
    SHARED_FUNCTION bool Initialise(Smp::ISimulator* simulator, Smp::Publication::ITypeRegistry* tReg) {
        simulator->RegisterFactory(new simph::kern::Factory<simph::umdl::SimTimeProvider>(
                "SimTimeProvider_Factory","",simulator,
                            {0x2adfaf7c,0x42ab,0x11e9,0x9feb,0x0f29cca3fa03}));
        simulator->RegisterFactory(new simph::kern::Factory<simph::umdl::SysTimeSynchro>(
                "SysTimeSynchro_Factory","",simulator,
                            {0x969a960a,0x42ab,0x11e9,0x8aa2,0x270a7d28d960}));
        simulator->RegisterFactory(new simph::kern::Factory<simph::umdl::Logger1D>(
                "Logger1D_Factory","",simulator,
                            {0xd9e94d0c,0x42ab,0x11e9,0x8b8e,0x4bc383f9f8bc}));
        return true;
    }
    SHARED_FUNCTION bool Finalise() {
        return true;
    }
}
