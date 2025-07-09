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
    }
}
// ..........................................................
EventManager::~EventManager() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::Services::EventId EventManager::QueryEventId(Smp::String8 eventName) {
    for (int i = 1; i <= Smp::Services::IEventManager::SMP_PostSimTimeChangeId; ++i) {
        if (strcmp(eventName, _SMP_EventNamesTable[i - 1]) == 0) {
            return i;
        }
    }
    const char* c = eventName;
    // max uint64 to avoid an overflow error when multiplying by 31
    uint64_t maxU64 = std::numeric_limits<uint64_t>::max() >> 4;
    Smp::Services::EventId id = 32;
    while (*c != '\0') {
        id = (id % maxU64) * 31 + (int)(*c);
        c++;
    }
    auto itEps = _evRegistry.find(id);
    if (itEps == _evRegistry.end()) {
        // Create new slot in registry for the queried event if not yet
        // existing.
        _evRegistry.emplace(std::piecewise_construct, std::forward_as_tuple(id),
                            std::forward_as_tuple());
    }
    // TODO may be it is possible to check for collisition by checking
    // collection's name against provided event name when there is already
    // something in the registry with the same id.
    return id;
}
// ..........................................................
void EventManager::Subscribe(Smp::Services::EventId event, const Smp::IEntryPoint* entryPoint) {
    // TODO make it thread safe.
    // Or restrict use from a single scheduler...
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
    // TODO make it thread safe.
    // Or restrict use from a single scheduler...
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
