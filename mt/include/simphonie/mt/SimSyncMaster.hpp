/*
 * @file SimSyncMaster.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_mt_SimSyncMaster_HPP__
#define __simphonie_mt_SimSyncMaster_HPP__

#include "Smp/ISimulator.h"
#include "simdeck/Service.hpp"
#include "simdeck/EntryPointPublisher.hpp"
#include "simphonie/mt/SimSyncSlave.hpp"
#include "simphonie/sys/Barrier.hpp"

namespace simphonie {
namespace mt {

class SimSyncMaster : public simdeck::Service, public simdeck::EntryPointPublisher {
public:
    SimSyncMaster(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    ~SimSyncMaster() = default;

protected:
    void publish(Smp::IPublication* receiver) override;
    void configure() override;
    void connect() override;

private:
    void sync();
    void init();
    void store();
    void restore();
    void run();
    void leaverun();
    void exit();
    void abort();

    simphonie::mt::SimSyncSlave* _slavesAddr[1];
    std::vector<SimSyncSlave*> _slaves;
    simphonie::sys::Barrier _barrier;

};

}  /* namespace mt */
}  /* namespace simphonie */
#endif  /* __simphonie_mt_SimSyncMaster_HPP__ */
