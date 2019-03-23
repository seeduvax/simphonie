/*
 * @file EventManager.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/EventManager.hpp"
#include "simph/kern/ExInvalidEventId.hpp"
#include "simph/kern/ExEntryPointAlreadySubscribed.hpp"
#include "simph/kern/ExEntryPointNotSubscribed.hpp"
#include "simph/sys/Logger.hpp"
namespace Smp {
    namespace Services {
constexpr Smp::Services::EventId IEventManager::SMP_LeaveConnectingId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterInitialisingId;
constexpr Smp::Services::EventId IEventManager::SMP_LeaveInitialisingId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterStandbyId;
constexpr Smp::Services::EventId IEventManager::SMP_LeaveStandbyId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterExecutingId;
constexpr Smp::Services::EventId IEventManager::SMP_LeaveExecutingId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterStoringId;
constexpr Smp::Services::EventId IEventManager::SMP_LeaveStoringId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterRestoringId;
constexpr Smp::Services::EventId IEventManager::SMP_LeaveRestoringId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterExitingId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterAbortingId;
constexpr Smp::Services::EventId IEventManager::SMP_EpochTimeChangedId;
constexpr Smp::Services::EventId IEventManager::SMP_MissionTimeChangedId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterReconnectingId;
constexpr Smp::Services::EventId IEventManager::SMP_LeaveReconnectingId;
constexpr Smp::Services::EventId IEventManager::SMP_PreSimTimeChangeId;
constexpr Smp::Services::EventId IEventManager::SMP_PostSimTimeChangeId;
constexpr Smp::Char8 IEventManager::SMP_EventManager[];
constexpr Smp::Char8 IEventManager::SMP_LeaveConnecting[];
constexpr Smp::Char8 IEventManager::SMP_EnterInitialising[];
constexpr Smp::Char8 IEventManager::SMP_LeaveInitialising[];
constexpr Smp::Char8 IEventManager::SMP_EnterStandby[];
constexpr Smp::Char8 IEventManager::SMP_LeaveStandby[];
constexpr Smp::Char8 IEventManager::SMP_EnterExecuting[];
constexpr Smp::Char8 IEventManager::SMP_LeaveExecuting[];
constexpr Smp::Char8 IEventManager::SMP_EnterStoring[];
constexpr Smp::Char8 IEventManager::SMP_LeaveStoring[];
constexpr Smp::Char8 IEventManager::SMP_EnterRestoring[];
constexpr Smp::Char8 IEventManager::SMP_LeaveRestoring[];
constexpr Smp::Char8 IEventManager::SMP_EnterExiting[];
constexpr Smp::Char8 IEventManager::SMP_EnterAborting[];
constexpr Smp::Char8 IEventManager::SMP_EpochTimeChanged[];
constexpr Smp::Char8 IEventManager::SMP_MissionTimeChanged[];
constexpr Smp::Char8 IEventManager::SMP_EnterReconnecting[];
constexpr Smp::Char8 IEventManager::SMP_LeaveReconnecting[];
constexpr Smp::Char8 IEventManager::SMP_PreSimTimeChange[];
constexpr Smp::Char8 IEventManager::SMP_PostSimTimeChange[];
    }
}

namespace simph {
	namespace kern {
const Smp::String8 _SMP_EventNamesTable[]={
    Smp::Services::IEventManager::SMP_LeaveConnecting,
    Smp::Services::IEventManager::SMP_EnterInitialising,
    Smp::Services::IEventManager::SMP_LeaveInitialising,
    Smp::Services::IEventManager::SMP_EnterStandby,
    Smp::Services::IEventManager::SMP_LeaveStandby,
    Smp::Services::IEventManager::SMP_EnterExecuting,
    Smp::Services::IEventManager::SMP_LeaveExecuting,
    Smp::Services::IEventManager::SMP_EnterStoring,
    Smp::Services::IEventManager::SMP_LeaveStoring,
    Smp::Services::IEventManager::SMP_EnterRestoring,
    Smp::Services::IEventManager::SMP_LeaveRestoring,
    Smp::Services::IEventManager::SMP_EnterExiting,
    Smp::Services::IEventManager::SMP_EnterAborting,
    Smp::Services::IEventManager::SMP_EpochTimeChanged,
    Smp::Services::IEventManager::SMP_MissionTimeChanged,
    Smp::Services::IEventManager::SMP_EnterReconnecting,
    Smp::Services::IEventManager::SMP_LeaveReconnecting,
    Smp::Services::IEventManager::SMP_PreSimTimeChange,
    Smp::Services::IEventManager::SMP_PostSimTimeChange
};
// --------------------------------------------------------------------
// ..........................................................
EventManager::EventManager(Smp::String8 name, Smp::String8 descr,Smp::IObject* parent):
                Component(name[0]=='\0'?Smp::Services::IEventManager::SMP_EventManager:name,
                          descr,parent) {
    for (int evtIdx=0;evtIdx<=Smp::Services::IEventManager::SMP_PostSimTimeChangeId;++evtIdx) {
        std::ostringstream cname;
        cname <<"events"<<evtIdx;
        _evRegistry.emplace(std::piecewise_construct,
                std::forward_as_tuple(evtIdx),
                std::forward_as_tuple(cname.str().c_str(),"",this));
    }
}
// ..........................................................
EventManager::~EventManager() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::Services::EventId EventManager::QueryEventId(Smp::String8 eventName) {
    for (int i=1;i<=Smp::Services::IEventManager::SMP_PostSimTimeChangeId;++i) {
        if (strcmp(eventName,_SMP_EventNamesTable[i-1])==0) {
            return i;
        }
    }
    return 0;
}
// ..........................................................
void EventManager::Subscribe(Smp::Services::EventId event,
                        const Smp::IEntryPoint* entryPoint) {
    // TODO make it thread safe.
    if (!entryPoint) {
        auto itEps=_evRegistry.find(event);
        if (itEps != _evRegistry.end() ) {
            if (itEps->second.contain(entryPoint)) {
                throw ExEntryPointAlreadySubscribed(this,entryPoint,
                        _SMP_EventNamesTable[itEps->first]);
            }
            itEps->second.push_back(entryPoint);
        } else {
            throw ExInvalidEventId(this,event);
        }
    }
}
// ..........................................................
void EventManager::Unsubscribe(Smp::Services::EventId event,
                        const Smp::IEntryPoint* entryPoint) {
    // TODO make it thread safe.
    auto itEps=_evRegistry.find(event);
    if (itEps != _evRegistry.end() ) {
        bool res = itEps->second.remove(entryPoint);
        if (!res) {
            new ExEntryPointNotSubscribed(this,entryPoint,
                        _SMP_EventNamesTable[event]);
        }
    } else {
        throw ExInvalidEventId(this,event);
    }
}
// ..........................................................
void EventManager::Emit(Smp::Services::EventId event,
                    Smp::Bool synchronous) {
    // TODO take care of synchronous.
    if (!synchronous) {
        LOGW("EventManager asynchronous Emit mode not implemented, handling synchronously.")
    }
    // TODO add some flag and checks to forbid recursive Emit
    auto itEps=_evRegistry.find(event);
    if (itEps != _evRegistry.end() ) {
        for (auto ep: itEps->second) {
            ep->Execute();
        }
    } else {
        throw ExInvalidEventId(this,event);
    }



}

}} // namespace simph::kern
