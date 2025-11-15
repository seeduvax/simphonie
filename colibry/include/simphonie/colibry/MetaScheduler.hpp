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

#define SMP_DURATION_MAX INT64_MAX

namespace simphonie {
namespace colibry {

/**
 *
 */
class MetaScheduler: public simdeck::CompositeService {
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


    class Schedule: public Smp::IObject {
    public:
        Schedule(MetaScheduler* mScheduler,
                Smp::IEntryPoint* ep);
        Schedule(MetaScheduler* mScheduler,
                Smp::Services::EventId evId);
        virtual ~Schedule();
        inline Schedule& setSimulationTime(Smp::Duration simTime) {
            _simulationTime=simTime;
            return *this;
        }
        inline Schedule& setCycleTime(Smp::Duration cycleTime) {
            _cycleTime=cycleTime;
            return *this;
        }
        inline Schedule& setRepeat(Smp::Int64 repeat) {
            _repeat=repeat;
            return *this;
        }
        void submit();
        inline Smp::Services::EventId getEventId() {
            return _eventId;
        }
        void epActivate();
        void epDeactivate();
        // Smp::IObject implementation
        Smp::String8 GetName() const override;
        Smp::String8 GetDescription() const override;
        Smp::IObject* GetParent() const override;
        Smp::IObject* GetChild(Smp::String8) const override;
    private:
        MetaScheduler* _metaScheduler=nullptr;
        Smp::IEntryPoint* _ep=nullptr;
        Smp::Services::EventId _eventId=-1;
        Smp::Duration _simulationTime;
        Smp::Duration _cycleTime=0;
        Smp::Int64 _repeat=0;
        Smp::IEntryPoint* _epActivate;
        Smp::IEntryPoint* _epDeactivate;
    };

    class IScheduleListener: public Smp::IObject {
    public:
        virtual void notifyEpBegin(Schedule* s);
        virtual void notifyEpEnd(Schedule* s);
    };

    inline Schedule* getSchedule(Smp::Services::EventId ev) const {
        auto it=_schedList.find(ev);
        return it!=_schedList.end()?it->second:nullptr;
    }
    inline Schedule* newSchedule(Smp::IEntryPoint* ep) {
        Schedule* s=new Schedule(this, ep);
        _schedList[s->getEventId()]=s;
        return s;
    }

    inline Smp::Duration getMaxSimTime() {
        return SMP_DURATION_MAX - _timeKeeper->GetSimulationTime();
    }
    inline Smp::Services::IScheduler* getScheduler() {
        return _scheduler;
    }

protected:
    /**
     *  on connect perform the needed registrations.
     *   - find listeners among childs and/or link registry,
     *   - when at least one listener is found, subscribe to the pre/post entry
     *     point execution events emitted by the scheduler.
     */     
    void connect() override;

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
    /** list of schedule */
    std::map<Smp::Services::EventId, Schedule*> _schedList;
    /** list of registered schedule listeners */
    std::vector<IScheduleListener*> _listeners;
    /** Current schedule (from last event id fetched as current from the scheduler on preEpExec()) */
    Schedule* _currentSchedule=nullptr;
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


};

}} // namespace simphonie::colibry
#endif // __simphonie_colibry_MetaScheduler_HPP__
