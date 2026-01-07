/*
 * @file Synchronizer.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_colibry_Synchronization_HPP__
#define __simphonie_colibry_Synchronization_HPP__

#include <chrono>
#include <time.h>
#include "Smp/IPublication.h"
#include "Smp/IService.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simdeck/Component.hpp"
#include "simdeck/EntryPointPublisher.hpp"
#include "simphonie/sys/Synchro.hpp"

namespace simphonie {
namespace colibry {

/**
 * Real time simulation synchronisation service.
 *
 * The strategy is to schedule a periodic entry point that just wait for the
 * zulu time to reach the simulation time. To be more precise wait until 
 * zulu time being equal to zulu time at start plus simulation time. The 
 * condition to run real time simulation is then that all other entry points
 * total computation time being lesser than the zulu time synchronization
 * period. When this condition is not met, that is when the synchronization
 * entry point is activated at a zulu time after the expected zulu time to
 * wait for, the overflow count is incremented and no wait is performed in the
 * hope to recover a better margin on next synchronization step.
 *
 * The service publishes many fields for its configuration and monitoring:
 *  - `Margin` [output, Smp::Int64]: value of nanosecond time left before the
 *    expected zulu time of the last synchronization entry point activation.
 *  - `OverflowCount` [output, Smp::UInt64]: count of the too late 
 *    synchronization entry point activation.
 *  - `Period` [input, Smp::Duration]: synchronization period (synchronization
 *    cycle time). Default value is 10ms.
 *  - `StartSec` [input, Smp::Bool]: when true, try to release the simulation
 *    start on Zulu time round seconds.
 *  - `MarginThreshold`[input, Smp::Int64]: minimum margin requested until 
 *    next expected zulu time on synchronization entry point activation. Default
 *    value is 100ns since the synchronization process requires its own 
 *    computation time (not measures and host hardware dependent). 
 *
 * The synchronization entry point is scheduled periodically according the
 * `Period` input field value on the service Connect.
 */  
class Synchronizer : public simdeck::Component,
                     virtual public simdeck::EntryPointPublisher,
                     virtual public Smp::IService {
public:
    Synchronizer(Smp::String8 name, Smp::String8 description = "", Smp::IObject* parent = nullptr);
    ~Synchronizer() = default;

    /**
     * Synchronization entry point.
     * Wait for zulu time reach the expected value or count overflow when the
     * computed margin is lower than the threshold.
     */ 
    void epStep();

protected:
    void connect() override;
    void publish(Smp::IPublication* receiver) override;

private:
    /**
     * Initialization entry point.
     * Compute the first expected synchronization zulu time. Optionally
     * wait for round second zulu time value.
     */ 
    void epInit();

    /** synchronization period */
    Smp::Duration _periodSmp;
    /** count of overflow events since simulation start */
    Smp::UInt64 _overflowCount;
    /** margin until expected zulu simulation time on synchronization */
    Smp::Int64 _margin;
    /** minimum margin required to not consider being too late and count
     *  overflow */
    Smp::Int64 _marginThresh;
    /** round second wait at start feature activation flag. */
    Smp::Bool _startSec;
    /** next expected zulu time for synchronization */
    std::chrono::time_point<std::chrono::system_clock> _goal;
};

} /* namespace colibry */
} /* namespace simphonie */
#endif /* __simphonie_colibry_Synchronization_HPP__ */
