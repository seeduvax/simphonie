/*
 * @file MetaScheduler.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_colibry_MetaScheduler_HPP__
#define __simphonie_colibry_MetaScheduler_HPP__

#include <map>
#include <vector>
#include "simdeck/CompositeService.hpp"
#include "Smp/Services/IScheduler.h"
#include "Smp/Services/ITimeKeeper.h"
#include "Smp/ISimulator.h"
#include "Smp/Services/IEventManager.h"
#include "simdeck/smpext/IMetaScheduler.hpp"

#define SMP_DURATION_MAX INT64_MAX

namespace simphonie {
namespace colibry {

/**
 *
 */
class MetaScheduler: public simdeck::CompositeService,
            virtual public simdeck::smpext::IMetaScheduler {
    typedef simdeck::CompositeService Parent;
public:
    /**
     * Default constructor.
     */
    MetaScheduler(Smp::String8 name, Smp::String8 description,
                  Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~MetaScheduler();
    class BaseSchedule;
    class Schedule;
    class ImmediateSchedule;

    // IMetaScheduler implementation
    ISchedule* GetSchedule(Smp::Services::EventId ev) const override {
        auto it=_schedIndex.find(ev);
        return it!=_schedIndex.end()?it->second:nullptr;
    }
    simdeck::smpext::IMetaScheduler::ISchedule* NewSchedule(Smp::IEntryPoint* ep, Smp::Bool immediate=false) override;


protected:
    /**
     *  on connect perform the needed registrations.
     *   - find listeners among childs and/or link registry,
     *   - when at least one listener is found, subscribe to the pre/post entry
     *     point execution events emitted by the scheduler.
     */     
    void connect() override;
    /**
     * in disconnect, unsubscribe. to all events 
     */     
    void disconnect() override;

    /** 
     * Action to be done before a scheduled entry point execution begins. 
     * Retrieve schedule and forward notification to all listeners.
     */
    void epPreEpExec();
    /** 
     * Action to be done just after a scheduled entry point execution ends.
     * forward notification to all listeners.
     */
    void epPostEpExec();


private:
    /** schedule index by event id */
    std::map<Smp::Services::EventId, simdeck::smpext::IMetaScheduler::ISchedule*> _schedIndex;
    /** list of all created schedule */
    std::vector<simdeck::smpext::IMetaScheduler::ISchedule*> _allSched;
    /** list of registered schedule listeners */
    std::vector<simdeck::smpext::IMetaScheduler::IScheduleListener*> _listeners;
    /** Current schedule (from last event id fetched as current from the scheduler on preEpExec()) */
    simdeck::smpext::IMetaScheduler::ISchedule* _currentSchedule=nullptr;
    /** Pre entry point execute event handler */
    Smp::IEntryPoint* _epPreEpExec;
    /** Post entry point execute event handler */
    Smp::IEntryPoint* _epPostEpExec;
    /** 
     * reference to the scheduler for later use and forward to the internal 
     * Schedule objects
     */
    Smp::Services::IScheduler* _scheduler=nullptr;
    /**
     * reference to the timekeeper for later use
     */
    Smp::Services::ITimeKeeper* _timeKeeper=nullptr;
    // internal utility methods.
    inline void registerSchedule(simdeck::smpext::IMetaScheduler::ISchedule* s) {
        auto id=s->GetEventId();
        if (id!=-1) {
            _schedIndex[id]=s;
        }
    }
    inline void registerSchedule(Smp::Services::EventId eventId, simdeck::smpext::IMetaScheduler::ISchedule* s) {
        _schedIndex[eventId]=s;
    }
};

}} // namespace simphonie::colibry
#endif // __simphonie_colibry_MetaScheduler_HPP__
