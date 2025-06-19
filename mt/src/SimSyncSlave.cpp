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

SimSyncDataShare::DataType SimSyncSlave::sendData() {
    std::unique_lock<std::mutex> lock(_dataMtx);
    return _data;
}

void SimSyncSlave::publish(Smp::IPublication* receiver) {
    _dataShare.publish(this, receiver);
}

void SimSyncSlave::sync() {
    {
        const auto data = _dataShare.retrieveData();
        std::unique_lock<std::mutex> lock(_dataMtx);
        _data = data;
    }
    if (_barrier->wait()) {
        const auto data = _masterSendData();
        _dataShare.loadData(data);
    }
    else {
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

} /* namespace mt */
} /* namespace simphonie */
