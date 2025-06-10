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

#include <sstream>

namespace simphonie {
namespace mt {

SimSyncSlave::SimSyncSlave(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent) :
    simdeck::Service(name, descr, parent), _exitFlag(EXIT)
{
    addEP("sync",
          "Simulator's synchronization function. Blocks until every simulators are waiting to then share data among "
          "them.",
          this, &SimSyncSlave::sync);
}

void SimSyncSlave::sync() {
    const auto start = std::chrono::system_clock::now();
    if (_barrier->wait()) {
        const auto end = std::chrono::system_clock::now();
        std::ostringstream oss;
        oss << (end - start).count();
        logDebug(oss.str().c_str());
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
