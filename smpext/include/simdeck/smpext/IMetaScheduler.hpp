/*
 * @file IMetaScheduler.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_smpext_IMetaScheduler_HPP__
#define __simdeck_smpext_IMetaScheduler_HPP__

#include "Smp/IService.h"
#include "Smp/Services/EventId.h"
#include "Smp/IEntryPoint.h"

namespace simdeck {
namespace smpext {

/**
 *
 */
class IMetaScheduler: public virtual Smp::IService  {
public:
    /**
     * Destructor.
     */
    virtual ~IMetaScheduler() noexcept = default;

    class ISchedule {
    public:
        virtual ~ISchedule() noexcept = default;
        virtual ISchedule& SetSimulationTime(Smp::Duration simTime) = 0;
        virtual ISchedule& SetCycleTime(Smp::Duration cycleTime) = 0;
        virtual ISchedule& SetRepeat(Smp::Int64 repeat) = 0;
        virtual ISchedule& SetActive(bool active) = 0;
        virtual ISchedule& SubscribeActivateEvent(Smp::Services::EventId event) = 0;
        virtual ISchedule& UnsubscribeActivateEvent(Smp::Services::EventId event) = 0;
        virtual ISchedule& SubscribeDeactivateEvent(Smp::Services::EventId event) = 0;
        virtual ISchedule& UnsubscribeDeactivateEvent(Smp::Services::EventId event) = 0;
        virtual void Submit() = 0;
        virtual Smp::Services::EventId GetEventId() const  = 0;
    };

    class IScheduleListener {
    public:
        virtual ~IEntryPointExecListener() noexcept = default;
        void NotifyExecBegin(const ISchedule* s) = 0;
        void NotifyExecEnd(const ISchedule* s) = 0;
    };

    virtual ISchedule* GetSchedule(Smp::Services::EventId id) const = 0;
    virtual ISchedule* NewSchedule(Smp::IentryPoint* ep) = 0;
};

}} // namespace simdeck::smpext
#endif // __simdeck_smpext_IMetaScheduler_HPP__
