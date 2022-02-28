/*
 * @file SyncSubSim.hpp
 *
 * Copyright 2022 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_mt_SyncSubSim_HPP__
#define __simph_mt_SyncSubSim_HPP__

#include <pthread.h>
#include "Smp/IComposite.h"
#include "Smp/IService.h"
#include "Smp/ISimulator.h"
#include "simph/smpdk/Collection.hpp"
#include "simph/smpdk/Component.hpp"
#include "simph/smpdk/Composite.hpp"

namespace simph {
namespace mt {

/**
 *
 */
class SyncSubSim : public simph::smpdk::Component, public virtual Smp::IComposite, public virtual Smp::IService {
public:
    /**
     * Default constructor.
     */
    SyncSubSim(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~SyncSubSim();

    void syncEP();
    void syncInitEP();
    void syncReleaseEP();

    // IComposite implementation
    const Smp::ContainerCollection* GetContainers() const override;
    Smp::IContainer* GetContainer(Smp::String8 name) const override;

protected:
    void publish(Smp::IPublication* receiver) override;
    void configure() override;
    void connect() override;

private:
    Smp::ISimulator* _subSim;
    Smp::Duration _syncRate;
    simph::smpdk::CollectionOwner<Smp::IContainer> _containers;
    pthread_barrier_t _barrier;

    Smp::Services::EventId _evSyncMaster;
    Smp::Services::EventId _evSyncSub;
    Smp::IEntryPoint* _epSync;
};

}  // namespace mt
}  // namespace simph
#endif  // __simph_mt_SyncSubSim_HPP__
