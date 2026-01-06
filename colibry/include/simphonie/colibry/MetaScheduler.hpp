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


    class Schedule: public Smp::IObject, 
            virtual public simdeck::smpext::IMetaScheduler::ISchedule {
    public:
        Schedule(MetaScheduler* mScheduler,
                Smp::IEntryPoint* ep);
        Schedule(MetaScheduler* mScheduler,
                Smp::Services::EventId evId);
        virtual ~Schedule();
        Schedule& SetSimulationTime(Smp::Duration simTime) override {
            _simulationTime=simTime;
            return *this;
        }
        Schedule& SetCycleTime(Smp::Duration cycleTime) override {
            _cycleTime=cycleTime;
            return *this;
        }
        Schedule& SetRepeat(Smp::Int64 repeat) override {
            _repeat=repeat;
            _repeatChanged=true;
            return *this;
        }
        Schedule& SetActive(bool active) override {
            _active=active;
            return *this;
        }
        Schedule& SubscribeActivateEvent(Smp::Services::EventId event) override {
            _metaScheduler->getSimulator()->GetEventManager()->Subscribe(event,_epActivate);
            return *this;
        }
        Schedule& UnsubscribeActivateEvent(Smp::Services::EventId event) override {
            _metaScheduler->getSimulator()->GetEventManager()->Unsubscribe(event,_epActivate);
            return *this;
        }
        Schedule& SubscribeDeactivateEvent(Smp::Services::EventId event)  override {
            _metaScheduler->getSimulator()->GetEventManager()->Subscribe(event,_epDeactivate);
            return *this;
        }
        Schedule& UnsubscribeDeactivateEvent(Smp::Services::EventId event) override {
            _metaScheduler->getSimulator()->GetEventManager()->Unsubscribe(event,_epDeactivate);
            return *this;
        }
        void Submit() override;
        void SubmitImmediate() override;
        Smp::Services::EventId GetEventId() const override {
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
        inline Smp::Duration getMaxCycleTime() {
            return SMP_DURATION_MAX
                    - _metaScheduler->_timeKeeper->GetSimulationTime()
                    - _simulationTime;
        }
        MetaScheduler* _metaScheduler=nullptr;
        Smp::IEntryPoint* _ep=nullptr;
        Smp::Services::EventId _eventId=-1;
        Smp::Duration _simulationTime;
        bool _simulationTimeChanged=false;
        Smp::Duration _cycleTime=0;
        bool _cycleTimeChanged=false;
        Smp::Int64 _repeat=0;
        bool _repeatChanged=false;
        Smp::IEntryPoint* _epActivate;
        Smp::IEntryPoint* _epDeactivate;
        bool _active=true;
    };

    Schedule* GetSchedule(Smp::Services::EventId ev) const override {
        auto it=_schedList.find(ev);
        return it!=_schedList.end()?it->second:nullptr;
    }
    Schedule* NewSchedule(Smp::IEntryPoint* ep) override {
        Schedule* s=new Schedule(this, ep);
        return s;
    }
    inline Smp::Services::IScheduler* getScheduler() {
        return _scheduler;
    }
    inline void registerSchedule(Schedule* s) {
        auto id=s->GetEventId();
        if (id!=-1) {
            _schedList[id]=s;
        }
    }
    inline void registerSchedule(Smp::Services::EventId eventId, Schedule* s) {
        _schedList[eventId]=s;
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
    std::vector<simdeck::smpext::IMetaScheduler::IScheduleListener*> _listeners;
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
