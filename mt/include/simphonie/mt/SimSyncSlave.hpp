/*
 * @file SimSyncSlave.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_mt_SimSyncSlave_HPP__
#define __simphonie_mt_SimSyncSlave_HPP__
#include "Smp/ISimulator.h"
#include "simdeck/Service.hpp"
#include "simdeck/EntryPointPublisher.hpp"
#include "simphonie/sys/Barrier.hpp"

namespace simphonie {
namespace mt {

class SimSyncSlave : public simdeck::Service, public simdeck::EntryPointPublisher {
public:
    enum exitFlags {
        HOLD,
        EXIT,
        ABORT
    };

    SimSyncSlave(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    ~SimSyncSlave() = default;

    inline Smp::ISimulator* getSim() { return getSimulator(); }
    inline void setBarrier(simphonie::sys::Barrier *barrier) { _barrier = barrier; }
    inline void setExitFlag(exitFlags flag) { _exitFlag = flag; }

private:
    void sync();
    simphonie::sys::Barrier *_barrier;
    exitFlags _exitFlag;

};

}  /* namespace mt */
}  /* namespace simphonie */
#endif  /* __simphonie_mt_SimSyncSlave_HPP__ */
