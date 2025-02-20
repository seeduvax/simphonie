/*
 * @file SyncSubSim.hpp
 *
 * Copyright 2022 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_mt_SyncSubSim_HPP__
#define __simphonie_mt_SyncSubSim_HPP__

#include <pthread.h>
#include "Smp/IService.h"
#include "Smp/ISimulator.h"
#include "Smp/IModel.h"
#include "Smp/Services/EventId.h"
#include "simdeck/Collection.hpp"
#include "simdeck/Component.hpp"
#include "simdeck/Composite.hpp"

namespace simphonie {
namespace mt {

/**
 *
 */
class SyncSubSim : public simdeck::Component,
                    public virtual Smp::IModel,
                    public virtual Smp::IService {
public:
    /**
     * Default constructor.
     */
    SyncSubSim(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~SyncSubSim();


    inline Smp::ISimulator* getSubSim() {
        return _subSim;
    }

protected:
    void publish(Smp::IPublication* receiver) override;
    void configure() override;
    void connect() override;

private:
    Smp::ISimulator* _subSim;
    Smp::Duration _syncRate;
    pthread_barrier_t _barrier;
    void epSync();
    void epSyncInit();
    void epRun();
    void epHold();

    Smp::Services::EventId _evSyncMaster;
    Smp::Services::EventId _evSyncSub;
    Smp::IEntryPoint* _initEP;
    Smp::IEntryPoint* _syncEP;
    Smp::IEntryPoint* _enterExecutingEP;
    Smp::IEntryPoint* _leaveExecutingEP;
};

}  // namespace mt
}  // namespace simph
#endif  // __simphonie_mt_SyncSubSim_HPP__
