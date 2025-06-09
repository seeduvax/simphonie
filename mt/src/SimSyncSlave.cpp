/*
 * @file SimSyncSlave.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/mt/SimSyncSlave.hpp"
#include "simdeck/ExInvalidParent.hpp"

#include <thread>

namespace simphonie {
namespace mt {

SimSyncSlave::SimSyncSlave(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent) :
    simdeck::Service(name, descr, parent), _exitFlag(EXIT)
{
    addEP("sync", "Simulators' synchronization function. Blocks until every slaveulators are waiting to then share data among them.", this, &SimSyncSlave::sync);
}

void SimSyncSlave::sync() {
    if (_barrier->wait()) {
        /* Share data here */
    } else { 
        switch (_exitFlag) {
        case HOLD:
            getSimulator()->Hold(true);
            break;
        case EXIT:
            getSimulator()->Exit();
            break;
        case ABORT:
            getSimulator()->Abort();
            break;
        }
    }
}

}  /* namespace mt */
}  /* namespace simphonie */
