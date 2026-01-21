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


    class BaseSchedule: public Smp::IObject, 
            virtual public simdeck::smpext::IMetaScheduler::ISchedule {
    public: 
        BaseSchedule(MetaScheduler* mScheduler, Smp::IEntryPoint* ep);
        BaseSchedule(MetaScheduler* mScheduler, Smp::Services::EventId eventId);
        virtual ~BaseSchedule();
        ISchedule& SetActive(Smp::Bool active) override {
            _active=active;
            return *this;
        }
        ISchedule& SetSimulationTime(Smp::Duration simTime) override {
            // ignore request, not applicable to base and immediate event.
            return *this;
        }
        ISchedule& SetCycleTime(Smp::Duration cycleTime) override {
            // ignore request, not applicable to base and immediate event.
            return *this;
        }
        ISchedule& SetRepeat(Smp::Int64 repeat) override {
            // ignore request, not applicable to base and immediate event.
            return *this;
        }
        Smp::Bool IsActive() override {
            return _active;
        }
        Smp::Services::EventId GetEventId() const override {
            return _eventId;
        }
        ISchedule& SubscribeActivateEvent(Smp::Services::EventId event) override {
            _metaScheduler->getSimulator()->GetEventManager()->Subscribe(event, _epActivate);
            return *this;
        }
        ISchedule& UnsubscribeActivateEvent(Smp::Services::EventId event) override {
            _metaScheduler->getSimulator()->GetEventManager()->Unsubscribe(event, _epActivate);
            return *this;
        }
        ISchedule& SubscribeDeactivateEvent(Smp::Services::EventId event)  override {
            _metaScheduler->getSimulator()->GetEventManager()->Subscribe(event, _epDeactivate);
            return *this;
        }
        ISchedule& UnsubscribeDeactivateEvent(Smp::Services::EventId event) override {
            _metaScheduler->getSimulator()->GetEventManager()->Unsubscribe(event, _epDeactivate);
            return *this;
        }
        Smp::Bool IsImmediate() override {
            return false;
        }
        Smp::Duration GetSimulationTime() override {
            return -1;
        }
        Smp::Duration GetCycleTime() override {
            return -1;
        }
        Smp::Int64 GetRepeat() override {
            return 0;
        }
        void Submit() override {
        }
        // Smp::IObject implementation
        Smp::String8 GetName() const override;
        Smp::String8 GetDescription() const override;
        Smp::IObject* GetParent() const override;
        Smp::IObject* GetChild(Smp::String8) const override;
    protected:
        inline Smp::IEntryPoint* getEntryPoint() {
            return _ep;
        }
        inline void setEventId(Smp::Services::EventId id) {
            _eventId=id;
        }
        inline MetaScheduler* getMetaScheduler() {
            return _metaScheduler;
        }
        virtual void epActivate();
        virtual void epDeactivate();
    private:
        Smp::IEntryPoint* _ep=nullptr;
        Smp::Services::EventId _eventId=-1;
        bool _active=true;
        MetaScheduler* _metaScheduler=nullptr;
        Smp::IEntryPoint* _epActivate;
        Smp::IEntryPoint* _epDeactivate;

    };
    class Schedule: public BaseSchedule {
    public:
        Schedule(MetaScheduler* mScheduler,
                Smp::IEntryPoint* ep);
        virtual ~Schedule();
        Schedule& SetSimulationTime(Smp::Duration simTime) override {
            _simulationTimeChanged=simTime!=_simulationTime;
            _simulationTime=simTime;
            return *this;
        }
        Schedule& SetCycleTime(Smp::Duration cycleTime) override {
            _cycleTimeChanged=cycleTime!=_cycleTime;
            _cycleTime=cycleTime;
            return *this;
        }
        Schedule& SetRepeat(Smp::Int64 repeat) override {
            _repeatChanged=_repeat!=repeat;
            _repeat=repeat;
            return *this;
        }
        void Submit() override;
        Smp::Duration GetSimulationTime() override {
            return _simulationTime;
        }
        Smp::Duration GetCycleTime() override {
            return _cycleTime;
        }
        Smp::Int64 GetRepeat() override {
            return _repeat;
        }
    protected:
        void epActivate() override;
        void epDeactivate() override;
    private:
        inline Smp::Duration getMaxCycleTime() {
            return SMP_DURATION_MAX
                    - getMetaScheduler()->_timeKeeper->GetSimulationTime()
                    - _simulationTime;
        }
        Smp::Duration _simulationTime;
        bool _simulationTimeChanged=false;
        Smp::Duration _cycleTime=0;
        bool _cycleTimeChanged=false;
        Smp::Int64 _repeat=0;
        bool _repeatChanged=false;
    };
    class ImmediateSchedule: public BaseSchedule {
    public:
        ImmediateSchedule(MetaScheduler* mScheduler, Smp::IEntryPoint* entryPoint);
        virtual ~ImmediateSchedule();
        void Submit() override;
        Smp::Bool IsImmediate() override {
            return false;
        }
    };

    ISchedule* GetSchedule(Smp::Services::EventId ev) const override {
        auto it=_schedList.find(ev);
        return it!=_schedList.end()?it->second:nullptr;
    }
    BaseSchedule* NewSchedule(Smp::IEntryPoint* ep, Smp::Bool immediate=false) override {
        BaseSchedule* s;
        if (immediate) {
            s=new ImmediateSchedule(this, ep);
        }
        else {
            s=new Schedule(this, ep);
        }
        return s;
    }
    inline Smp::Services::IScheduler* getScheduler() {
        return _scheduler;
    }
    inline void registerSchedule(BaseSchedule* s) {
        auto id=s->GetEventId();
        if (id!=-1) {
            _schedList[id]=s;
        }
    }
    inline void registerSchedule(Smp::Services::EventId eventId, BaseSchedule* s) {
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
    std::map<Smp::Services::EventId, simdeck::smpext::IMetaScheduler::ISchedule*> _schedList;
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


};

}} // namespace simphonie::colibry
#endif // __simphonie_colibry_MetaScheduler_HPP__
