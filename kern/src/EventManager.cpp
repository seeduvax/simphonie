/*
 * @file EventManager.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/EventManager.hpp"
#include "simphonie/kern/ExEntryPointAlreadySubscribed.hpp"
#include "simphonie/kern/ExEntryPointNotSubscribed.hpp"
#include "simphonie/kern/ExInvalidEventId.hpp"
#include "simphonie/kern/ExInvalidEventName.hpp"
#include "simphonie/sys/Logger.hpp"
#include "Smp/IEntryPoint.h"

namespace simphonie {
namespace kern {
const Smp::String8 _SMP_EventNamesTable[] = {
    Smp::Services::IEventManager::SMP_LeaveConnecting,    Smp::Services::IEventManager::SMP_EnterInitialising,
    Smp::Services::IEventManager::SMP_LeaveInitialising,  Smp::Services::IEventManager::SMP_EnterStandby,
    Smp::Services::IEventManager::SMP_LeaveStandby,       Smp::Services::IEventManager::SMP_EnterExecuting,
    Smp::Services::IEventManager::SMP_LeaveExecuting,     Smp::Services::IEventManager::SMP_EnterStoring,
    Smp::Services::IEventManager::SMP_LeaveStoring,       Smp::Services::IEventManager::SMP_EnterRestoring,
    Smp::Services::IEventManager::SMP_LeaveRestoring,     Smp::Services::IEventManager::SMP_EnterExiting,
    Smp::Services::IEventManager::SMP_EnterAborting,      Smp::Services::IEventManager::SMP_EpochTimeChanged,
    Smp::Services::IEventManager::SMP_MissionTimeChanged, Smp::Services::IEventManager::SMP_EnterReconnecting,
    Smp::Services::IEventManager::SMP_LeaveReconnecting,  Smp::Services::IEventManager::SMP_PreSimTimeChange,
    Smp::Services::IEventManager::SMP_PostSimTimeChange};
// --------------------------------------------------------------------
// ..........................................................
EventManager::EventManager(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Component(name, descr, parent) {
    for (int evtIdx = 1; evtIdx <= Smp::Services::IEventManager::SMP_PostSimTimeChangeId; ++evtIdx) {
        _evRegistry.emplace(std::piecewise_construct, std::forward_as_tuple(evtIdx),
                            std::forward_as_tuple());
        _idIndex[_SMP_EventNamesTable[evtIdx-1]]=evtIdx;
        _nextId++;
    }
}
// ..........................................................
EventManager::~EventManager() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::Services::EventId EventManager::QueryEventId(Smp::String8 eventName) {
    Synchronized(_mutex);
    Smp::Services::EventId id=0;
    if ( eventName[0] == '\0' ) {
        throw ExInvalidEventName(this);
    }
    auto it=_idIndex.find(eventName);
    if (it!=_idIndex.end()) {
        id = it->second;
    }
    else {
        _idIndex[eventName] = _nextId;
        id = _nextId;
        _evRegistry.emplace(std::piecewise_construct, std::forward_as_tuple(id),
                            std::forward_as_tuple());
        _nextId++;
    }
    return id;
}
// ..........................................................
void EventManager::Subscribe(Smp::Services::EventId event, const Smp::IEntryPoint* entryPoint) {
    Synchronized(_mutex);
    if (entryPoint != nullptr) {
        auto itEps = _evRegistry.find(event);
        if (itEps != _evRegistry.end()) {
            if (itEps->second.contain(entryPoint)) {
                throw ExEntryPointAlreadySubscribed(this, entryPoint, _SMP_EventNamesTable[itEps->first]);
            }
            if ( _emitting ) {
                // Event emit in progress, registration requiest is OK, add to 
                // registration list, to be processed later.
                SubQuery sq={event, true, entryPoint};
                _subQueries.push_back(sq);
            }
            else {
                itEps->second.push_back(entryPoint);
            }
        }
        else {
            throw ExInvalidEventId(this, event);
        }
    }
}
// ..........................................................
void EventManager::Unsubscribe(Smp::Services::EventId event, const Smp::IEntryPoint* entryPoint) {
    Synchronized(_mutex);
    auto itEps = _evRegistry.find(event);
    if (itEps != _evRegistry.end()) {
        bool res = itEps->second.contain(entryPoint);
        if (!res) {
            new ExEntryPointNotSubscribed(this, entryPoint, _SMP_EventNamesTable[event]);
        }
        if ( _emitting ) {
            // Event emit in progress, registration requiest is OK, add to 
            // registration list, to be processed later.
            SubQuery sq={event, false, entryPoint};
            _subQueries.push_back(sq);
        }
        else {
            itEps->second.remove(entryPoint);
        }
    }
    else {
        throw ExInvalidEventId(this, event);
    }
}
// ..........................................................
void EventManager::Emit(Smp::Services::EventId event, Smp::Bool synchronous) {
    // TODO take care of synchronous.
    if (!synchronous) {
        LOGW("EventManager asynchronous Emit mode not implemented, handling synchronously.")
    }
    {
        Synchronized(_mutex);
        _emitting=true;

    }
    // TODO add some flag and checks to forbid recursive Emit
    auto itEps = _evRegistry.find(event);
    if (itEps != _evRegistry.end()) {
        for (auto ep : itEps->second) {
            ep->Execute();
        }
    }
    else {
        throw ExInvalidEventId(this, event);
    }
    {
        Synchronized(_mutex);
        _emitting=false;
        // Once emit loop completed, process subscription requests receveived
        // during the emit loop.
        for (auto q: _subQueries) {
            auto it = _evRegistry.find(q._event);
            if (it != _evRegistry.end()) {
                if ( q._sub ) {
                    it->second.push_back(q._ep);
                }
                else {
                    it->second.remove(q._ep);
                }
            }
        }
        _subQueries.clear();
    }
}

}  // namespace kern
}  // namespace simph
