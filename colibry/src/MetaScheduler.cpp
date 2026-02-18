/*
 * @file MetaScheduler.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/colibry/MetaScheduler.hpp"
#include "simdeck/EntryPoint.hpp"


// TODO better string constant management: 2 next defines are copy past from 
// kern::Scheduler cpp file
#define EV_NAME_PRE_EVENT_EXECUTE "Scheduler_PreEventExecute"
#define EV_NAME_POST_EVENT_EXECUTE "Scheduler_PostEventExecute"
#define CONTAINER_NAME "Listeners"

namespace simphonie {
namespace colibry {

// --------------------------------------------------------------------
class MetaScheduler::BaseSchedule: public Smp::IObject, 
        virtual public simdeck::smpext::IMetaScheduler::ISchedule {
public: 
    BaseSchedule(MetaScheduler* mScheduler, Smp::IEntryPoint* ep):
            _metaScheduler(mScheduler),
            _ep(ep) {
        _epActivate=simdeck::EntryPoint::Create("scheduleActivate",
                                    "Schedule activation entry point", 
                                    this, &MetaScheduler::BaseSchedule::epActivate);
        _epDeactivate=simdeck::EntryPoint::Create("scheduleDeactivate",
                                    "Schedule deactivation entry point", 
                                    this, &MetaScheduler::BaseSchedule::epDeactivate);
    }
    BaseSchedule(MetaScheduler* mScheduler, Smp::Services::EventId evId):
            _metaScheduler(mScheduler),
            _eventId(evId) {
        _epActivate=simdeck::EntryPoint::Create("scheduleActivate",
                                    "Schedule activation entry point", 
                                    this, &MetaScheduler::BaseSchedule::epActivate);
        _epDeactivate=simdeck::EntryPoint::Create("scheduleDeactivate",
                                    "Schedule deactivation entry point", 
                                    this, &MetaScheduler::BaseSchedule::epDeactivate);
    }
    virtual ~BaseSchedule() {
        delete _epActivate;
        delete _epDeactivate;
    }
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
    ISchedule& SubscribeCycleTimeChangeEvent(
                        Smp::Services::EventId event,
                        Smp::Duration cycleTime,
                        Smp::Duration delay = 0) override {
        // ignore request, not applicable to base and immediate event.
        return *this;
    }
    ISchedule& UnsubscribeCycleTimeChangeEvent(
                        Smp::Services::EventId event) override {
        // ignore request, not applicable to base and immediate event.
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
    Smp::String8 GetName() const override {
        return _ep!=nullptr?_ep->GetName():"schedule";
    }
    Smp::String8 GetDescription() const override {
        return "Entry point schedule";
    }
    Smp::IObject* GetParent() const override {
        return _metaScheduler;
    }
    Smp::IObject* GetChild(Smp::String8) const override {
        return nullptr;
    }
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
    virtual void epActivate() {
        _active=true;
    }
    virtual void epDeactivate() {
        _active=false;
    }
private:
    Smp::IEntryPoint* _ep=nullptr;
    Smp::Services::EventId _eventId=-1;
    bool _active=true;
    MetaScheduler* _metaScheduler=nullptr;
    Smp::IEntryPoint* _epActivate;
    Smp::IEntryPoint* _epDeactivate;
};
// --------------------------------------------------------------------
class MetaScheduler::Schedule: public MetaScheduler::BaseSchedule {
public:
    Schedule(MetaScheduler* mScheduler, Smp::IEntryPoint* ep)
                : MetaScheduler::BaseSchedule(mScheduler, ep) {
    }
    virtual ~Schedule() {
        for (auto entry: _ccHandlers) {
            delete entry.second;
        }
    }
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
    Smp::Duration GetSimulationTime() override {
        return _simulationTime;
    }
    Smp::Duration GetCycleTime() override {
        return _cycleTime;
    }
    Smp::Int64 GetRepeat() override {
        return _repeat;
    }
    void Submit() override {
        if (GetEventId()==-1) {
            // first submit: add event to the SMP scheduler.
            setEventId(getMetaScheduler()->_scheduler->AddSimulationTimeEvent(
                getEntryPoint(),
                IsActive() ? _simulationTime : -1, 
                _cycleTime,
                _repeat));
            getMetaScheduler()->registerSchedule(this);
        }
        else {
            // evenet already submitted, update the schedule attributes.
            if (_simulationTimeChanged) {
                getMetaScheduler()->_scheduler->SetEventSimulationTime(GetEventId(), 
                    IsActive() ? _simulationTime : -1);
            }
            if (_cycleTimeChanged) {
                getMetaScheduler()->_scheduler->SetEventCycleTime(GetEventId(), _cycleTime);
            }
            if (_repeatChanged) {
                getMetaScheduler()->_scheduler->SetEventRepeat(GetEventId(), _repeat);
            }
        }
        _simulationTimeChanged=false;
        _cycleTimeChanged=false;
        _repeatChanged=false;
    }
    ISchedule& SubscribeCycleTimeChangeEvent(
                        Smp::Services::EventId event,
                        Smp::Duration cycleTime,
                        Smp::Duration delay = 0) override {
        auto it=_ccHandlers.find(event); 
        if (it == _ccHandlers.end()) {
            auto cch=new CycleChangeHandler(this, 
                                            getMetaScheduler()->getSimulator()->GetScheduler(),
                                            cycleTime, delay);
            getMetaScheduler()->getSimulator()->GetEventManager()->Subscribe(event, cch);
            _ccHandlers[event]=cch;
        }
        else {
            it->second->set(cycleTime, delay);
        }
        return *this;
    }
    ISchedule& UnsubscribeCycleTimeChangeEvent(
                        Smp::Services::EventId event) override {
        auto it=_ccHandlers.find(event);
        if (it != _ccHandlers.end()) {
            auto cch=it->second;
            getMetaScheduler()->getSimulator()->GetEventManager()->Unsubscribe(event, cch);
            _ccHandlers.erase(it);
            delete cch;
        }
        return *this;
    }
protected:
    class CycleChangeHandler: public Smp::IEntryPoint {
    public:
        CycleChangeHandler( ISchedule* schedule,
                            Smp::Services::IScheduler* smpScheduler,
                            Smp::Duration cycleTime,
                            Smp::Duration delay):
                _schedule(schedule),
                _smpScheduler(smpScheduler),
                _cycleTime(cycleTime),
                _delay(delay) {
        }
        virtual ~CycleChangeHandler() {
        }
        void Execute() const {
            if (_delay == 0 || _schedEvent == _smpScheduler->GetCurrentEventId()) {
                _schedule->SetCycleTime(_cycleTime).Submit();
                _schedEvent = -1;
            }
            else {
                if (_schedEvent != -1) {
                    // schedule at _delay - 1 to be sure that if schedule shall activate at _delay, cycle time is already changed.
                    _schedEvent = _smpScheduler->AddSimulationTimeEvent(this, _delay-1);
                }
                else {
                    // event received again during delay, then reschedule.
                    _smpScheduler->SetEventSimulationTime(_schedEvent, _delay-1);
                }
            }
        }
        void set(Smp::Duration cycleTime, Smp::Duration delay) {
            _cycleTime = cycleTime;
            _delay = delay;
        }
        // Smp::IObject implementation
        Smp::String8 GetName() const override {
            return "MetaSchedulerCycleChangeHandler";
        }
        Smp::String8 GetDescription() const override {
            return "Entry point handling on event cycle time change for the meta scheduler";
        }
        Smp::IObject* GetParent() const override {
            return nullptr;
        }
        Smp::IObject* GetChild(Smp::String8 name) const override {
            return nullptr;
        }
    private:
        simdeck::smpext::IMetaScheduler::ISchedule* _schedule;
        Smp::Services::IScheduler* _smpScheduler;
        Smp::Duration _cycleTime=0;
        Smp::Duration _delay=0;
        mutable Smp::Services::EventId _schedEvent = -1;
    };
    void epActivate() override {
        // TODO check SetEventSimulationTime also use relative time from now like
        // AddSimulationTimeEvent.
        MetaScheduler::BaseSchedule::epActivate();
        getMetaScheduler()->_scheduler->SetEventSimulationTime(GetEventId(),
                _simulationTime==-1?0:_simulationTime);
                // _simulationTime == -1 is related to an inactive schedule
                // event not explicitely defined with setActive(). On event,
                // such schedule shall be reset to run now (0 relative
                // simulation time)
    }
    void epDeactivate() override {
        MetaScheduler::BaseSchedule::epActivate();
        // Deactivate the event by setting its next activation time at -1
        getMetaScheduler()->_scheduler->SetEventSimulationTime(GetEventId(), -1);
    }
private:
    inline Smp::Duration getMaxCycleTime() {
        return SMP_DURATION_MAX
                - getMetaScheduler()->_timeKeeper->GetSimulationTime()
                - _simulationTime;
    }
    Smp::Duration _simulationTime=0;
    bool _simulationTimeChanged=false;
    Smp::Duration _cycleTime=0;
    bool _cycleTimeChanged=false;
    Smp::Int64 _repeat=0;
    bool _repeatChanged=false;
    std::map<Smp::Services::EventId, CycleChangeHandler*> _ccHandlers;
};
// --------------------------------------------------------------------
class MetaScheduler::ImmediateSchedule: public MetaScheduler::BaseSchedule {
public:
    ImmediateSchedule(MetaScheduler* mScheduler, Smp::IEntryPoint* ep)
                : MetaScheduler::BaseSchedule(mScheduler, ep) {
    }
    virtual ~ImmediateSchedule() {
    }
    Smp::Bool IsImmediate() override {
        return false;
    }
    void Submit() override {
        if (IsActive()) {
            // avoid this schedule to be store many times in the schedule list.
            auto& sl=getMetaScheduler()->_schedIndex;
            auto it=sl.find(GetEventId());
            if (it!=sl.end()) {
                sl.erase(it);
            }
            // when active add immediate event.
            setEventId(getMetaScheduler()->_scheduler->AddImmediateEvent(getEntryPoint()));
            getMetaScheduler()->registerSchedule(GetEventId(),this);
        }
    }
};


// --------------------------------------------------------------------
// ..........................................................
MetaScheduler::MetaScheduler(Smp::String8 name, Smp::String8 description, 
                Smp::IObject* parent): Parent(name, description, parent) {
    _epPreEpExec=simdeck::EntryPoint::Create("preEpExec",
                                "pre entry point execute event notification handler", 
                                this, &MetaScheduler::epPreEpExec);
    _epPostEpExec=simdeck::EntryPoint::Create("postEpExec",
                                "post entry point execute event notification handler", 
                                this, &MetaScheduler::epPostEpExec);
}
// ..........................................................

MetaScheduler::~MetaScheduler() {
    for (auto entry: _allSched) {
        delete entry;
    }
    auto evMgr=getSimulator()->GetEventManager();
    if (evMgr!=nullptr) {
/*
 * TODO check why this segfault in TestMetaScheduler
 * TODO service deletion order in the simulator may not be OK with that.
        evMgr->Unsubscribe(evMgr->QueryEventId(EV_NAME_PRE_EVENT_EXECUTE),_epPreEpExec);
        evMgr->Unsubscribe(evMgr->QueryEventId(EV_NAME_POST_EVENT_EXECUTE),_epPostEpExec);
*/
    }
    delete _epPreEpExec;
    delete _epPostEpExec;
}

// --------------------------------------------------------------------
// ..........................................................
void MetaScheduler::connect() {
    _scheduler=getSimulator()->GetScheduler();
    _timeKeeper=getSimulator()->GetTimeKeeper();
    auto evMgr=getSimulator()->GetEventManager();
    evMgr->Subscribe(evMgr->QueryEventId(EV_NAME_PRE_EVENT_EXECUTE),_epPreEpExec);
    evMgr->Subscribe(evMgr->QueryEventId(EV_NAME_POST_EVENT_EXECUTE),_epPostEpExec);

}
// ..........................................................
void MetaScheduler::epPreEpExec() {
    auto currentEventId=_scheduler->GetCurrentEventId();
    _currentSchedule=GetSchedule(currentEventId);
    if (_currentSchedule==nullptr) {
        // TODO find a way to retrieve entry point or schedule attributes from
        // the scheduler. 
        // S.Devaux: I fear SMP does not define anything to do so.
        _currentSchedule=new BaseSchedule(this,currentEventId);
        _allSched.push_back(_currentSchedule);
        _schedIndex[currentEventId]=_currentSchedule;
    }
    for (auto l: _listeners) {
        l->NotifyExecBegin(_currentSchedule);
    }
}
// ..........................................................
void MetaScheduler::epPostEpExec() {
    if (_currentSchedule!=nullptr) {
        for (auto l: _listeners) {
            l->NotifyExecEnd(_currentSchedule);
        }
        _currentSchedule=nullptr;
    }
    // TODO log error or warning when current schedule is nullptr. Such 
    // situation is totally unexpected since the current schedule should have
    // been retrieved or created on previous preEpExec and related pre/post
    // events should have been emitted seqentially one for one.
}
// ..........................................................
simdeck::smpext::IMetaScheduler::ISchedule* MetaScheduler::NewSchedule(Smp::IEntryPoint* ep, Smp::Bool immediate) {
    BaseSchedule* s;
    if (immediate) {
        s=new ImmediateSchedule(this, ep);
    }
    else {
        s=new Schedule(this, ep);
    }
    _allSched.push_back(s);
    return s;
}


}} // namespace simphonie::colibry
