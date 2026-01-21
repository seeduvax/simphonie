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
    for (auto entry: _schedList) {
        delete entry.second;
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
        _schedList[currentEventId]=_currentSchedule;
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
        if  (_currentSchedule->IsImmediate()) {
            auto it=_schedList.find(_currentSchedule->GetEventId());
            _schedList.erase(it);
        }
        _currentSchedule=nullptr;
    }
    // TODO log error or warning when current schedule is nullptr. Such 
    // situation is totally unexpected since the current schedule should have
    // been retrieved or created on previous preEpExec and related pre/post
    // events should have been emitted seqentially one for one.
}
// --------------------------------------------------------------------
// ..........................................................
MetaScheduler::BaseSchedule::BaseSchedule(MetaScheduler* mScheduler, Smp::IEntryPoint* ep):
        _metaScheduler(mScheduler),
        _ep(ep) {
    _epActivate=simdeck::EntryPoint::Create("scheduleActivate",
                                "Schedule activation entry point", 
                                this, &MetaScheduler::BaseSchedule::epActivate);
    _epDeactivate=simdeck::EntryPoint::Create("scheduleDeactivate",
                                "Schedule deactivation entry point", 
                                this, &MetaScheduler::BaseSchedule::epDeactivate);
}
// ..........................................................
MetaScheduler::BaseSchedule::BaseSchedule(MetaScheduler* mScheduler, Smp::Services::EventId evId):
        _metaScheduler(mScheduler),
        _eventId(evId) {
    _epActivate=simdeck::EntryPoint::Create("scheduleActivate",
                                "Schedule activation entry point", 
                                this, &MetaScheduler::BaseSchedule::epActivate);
    _epDeactivate=simdeck::EntryPoint::Create("scheduleDeactivate",
                                "Schedule deactivation entry point", 
                                this, &MetaScheduler::BaseSchedule::epDeactivate);
}
// ..........................................................
MetaScheduler::BaseSchedule::~BaseSchedule() {
    delete _epActivate;
    delete _epDeactivate;
}
// ..........................................................
MetaScheduler::Schedule::Schedule(MetaScheduler* mScheduler, Smp::IEntryPoint* ep)
            : MetaScheduler::BaseSchedule(mScheduler, ep) {
}
// ..........................................................
MetaScheduler::ImmediateSchedule::ImmediateSchedule(MetaScheduler* mScheduler, Smp::IEntryPoint* ep)
            : MetaScheduler::BaseSchedule(mScheduler, ep) {
}
// ..........................................................
MetaScheduler::Schedule::~Schedule() {
}
// ..........................................................
MetaScheduler::ImmediateSchedule::~ImmediateSchedule() {
}

// ..........................................................
void MetaScheduler::Schedule::Submit() {
    if (GetEventId()==-1) {
        // first submit: add event to the SMP scheduler.
        setEventId(getMetaScheduler()->getScheduler()->AddSimulationTimeEvent(
            getEntryPoint(),
            IsActive() ? _simulationTime : -1, 
            _cycleTime,
            _repeat));
        getMetaScheduler()->registerSchedule(this);
    }
    else {
        // evenet already submitted, update the schedule attributes.
        if (_simulationTimeChanged) {
            getMetaScheduler()->getScheduler()->SetEventSimulationTime(GetEventId(), 
                IsActive() ? _simulationTime : -1);
        }
        if (_cycleTimeChanged) {
            getMetaScheduler()->getScheduler()->SetEventCycleTime(GetEventId(), _cycleTime);
        }
        if (_repeatChanged) {
            getMetaScheduler()->getScheduler()->SetEventRepeat(GetEventId(), _repeat);
        }
    }
    _simulationTimeChanged=false;
    _cycleTimeChanged=false;
    _repeatChanged=false;
}
// ..........................................................
void MetaScheduler::ImmediateSchedule::Submit() {
    if (IsActive()) {
        // when active add immediate event.
        setEventId(getMetaScheduler()->getScheduler()->AddImmediateEvent(getEntryPoint()));
        getMetaScheduler()->registerSchedule(GetEventId(),this);
    }
}
// ..........................................................
void MetaScheduler::BaseSchedule::epActivate() {
    _active=true;
}
// ..........................................................
void MetaScheduler::Schedule::epActivate() {
    // TODO check SetEventSimulationTime also use relative time from now like
    // AddSimulationTimeEvent.
    MetaScheduler::BaseSchedule::epActivate();
    getMetaScheduler()->getScheduler()->SetEventSimulationTime(GetEventId(),
            _simulationTime==-1?0:_simulationTime);
            // _simulationTime == -1 is related to an inactive schedule
            // event not explicitely defined with setActive(). On event,
            // such schedule shall be reset to run now (0 relative
            // simulation time)
}
// ..........................................................
void MetaScheduler::BaseSchedule::epDeactivate() {
    _active=false;
}
// ..........................................................
void MetaScheduler::Schedule::epDeactivate() {
    MetaScheduler::BaseSchedule::epActivate();
    // Deactivate the event by setting its next activation time at -1
    getMetaScheduler()->getScheduler()->SetEventSimulationTime(GetEventId(), -1);
}
// ..........................................................
Smp::String8 MetaScheduler::BaseSchedule::GetName() const {
    return _ep!=nullptr?_ep->GetName():"";
}
Smp::String8 MetaScheduler::BaseSchedule::GetDescription() const {
    return "Entry point schedule";
}
Smp::IObject* MetaScheduler::BaseSchedule::GetParent() const {
    return _metaScheduler;
}
Smp::IObject* MetaScheduler::BaseSchedule::GetChild(Smp::String8) const {
    return nullptr;
}
}} // namespace simphonie::colibry
