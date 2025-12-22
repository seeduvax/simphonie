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
            itEps->second.push_back(entryPoint);
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
        bool res = itEps->second.remove(entryPoint);
        if (!res) {
            new ExEntryPointNotSubscribed(this, entryPoint, _SMP_EventNamesTable[event]);
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
}

}  // namespace kern
}  // namespace simph
