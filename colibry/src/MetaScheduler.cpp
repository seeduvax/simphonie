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
#include "Smp/ISimulator.h"
#include "Smp/Services/IEventManager.h"


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
        // TODO service deletion order in the simulator may not be OK with that.
        evMgr->Unsubscribe(evMgr->QueryEventId(EV_NAME_PRE_EVENT_EXECUTE),_epPreEpExec);
        evMgr->Unsubscribe(evMgr->QueryEventId(EV_NAME_POST_EVENT_EXECUTE),_epPostEpExec);
    }
    delete _epPreEpExec;
    delete _epPostEpExec;
}

// --------------------------------------------------------------------
// ..........................................................
void MetaScheduler::connect() {
    _scheduler=getSimulator()->GetScheduler();
    auto evMgr=getSimulator()->GetEventManager();
    evMgr->Subscribe(evMgr->QueryEventId(EV_NAME_PRE_EVENT_EXECUTE),_epPreEpExec);
    evMgr->Subscribe(evMgr->QueryEventId(EV_NAME_POST_EVENT_EXECUTE),_epPostEpExec);

}
// ..........................................................
void MetaScheduler::epPreEpExec() {
    auto currentEventId=_scheduler->GetCurrentEventId();
    _currentSchedule=getSchedule(currentEventId);
    if (_currentSchedule==nullptr) {
        // TODO find a way to retrieve entry point or schedule attributes from
        // the scheduler. 
        // S.Devaux: I fear SMP does not define anything to do so.
        _currentSchedule=new Schedule(this,currentEventId);
        _schedList[currentEventId]=_currentSchedule;
    }
    for (auto l: _listeners) {
        l->notifyEpBegin(_currentSchedule);
    }
}
// ..........................................................
void MetaScheduler::epPostEpExec() {
    if (_currentSchedule!=nullptr) {
        for (auto l: _listeners) {
            l->notifyEpEnd(_currentSchedule);
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
MetaScheduler::Schedule::Schedule(MetaScheduler* mScheduler, Smp::IEntryPoint* ep):
        _metaScheduler(mScheduler),
        _ep(ep) {
    _epActivate=simdeck::EntryPoint::Create("scheduleActivate",
                                "Schedule activation entry point", 
                                this, &MetaScheduler::Schedule::epActivate);
    _epDeactivate=simdeck::EntryPoint::Create("scheduleDeactivate",
                                "Schedule deactivation entry point", 
                                this, &MetaScheduler::Schedule::epDeactivate);
}
// ..........................................................
MetaScheduler::Schedule::Schedule(MetaScheduler* mScheduler, Smp::Services::EventId evId):
        _metaScheduler(mScheduler),
        _eventId(evId) {
    _epActivate=simdeck::EntryPoint::Create("scheduleActivate",
                                "Schedule activation entry point", 
                                this, &MetaScheduler::Schedule::epActivate);
    _epDeactivate=simdeck::EntryPoint::Create("scheduleDeactivate",
                                "Schedule deactivation entry point", 
                                this, &MetaScheduler::Schedule::epDeactivate);
}
// ..........................................................
MetaScheduler::Schedule::~Schedule() {
    delete _epActivate;
    delete _epDeactivate;
}
// ..........................................................
void MetaScheduler::Schedule::submit() {
    if (_eventId==-1) {
        _eventId=_metaScheduler->getScheduler()->AddSimulationTimeEvent(_ep, _simulationTime, _cycleTime, _repeat);
    }
    else {
        _metaScheduler->getScheduler()->SetEventSimulationTime(_eventId, _simulationTime);
        _metaScheduler->getScheduler()->SetEventCycleTime(_eventId, _cycleTime);
        _metaScheduler->getScheduler()->SetEventRepeat(_eventId, _repeat);
    }
}
// ..........................................................
void MetaScheduler::Schedule::epActivate() {
    // TODO check SetEventSimulationTime also use relative time from now like
    // AddSimulationTimeEvent.
    _metaScheduler->getScheduler()->SetEventSimulationTime(_eventId, _simulationTime);
}
// ..........................................................
void MetaScheduler::Schedule::epDeactivate() {
    // TODO check SetEventSimulationTime also use relative time from now like
    // AddSimulationTimeEvent.
    // Deactivate the event by setting its next activation time at the end of
    // simulation time.
    _metaScheduler->getScheduler()->SetEventSimulationTime(_eventId, _metaScheduler->getMaxSimTime());
}
// ..........................................................
Smp::String8 MetaScheduler::Schedule::GetName() const {
    return _ep!=nullptr?_ep->GetName():"";
}
Smp::String8 MetaScheduler::Schedule::GetDescription() const {
    return "Entry point schedule";
}
Smp::IObject* MetaScheduler::Schedule::GetParent() const {
    return _metaScheduler;
}
Smp::IObject* MetaScheduler::Schedule::GetChild(Smp::String8) const {
    return nullptr;
}
}} // namespace simphonie::colibry
