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
#include "simph/sys/Logger.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
EventManager::EventManager(Smp::String8 name, Smp::String8 descr,
                        Smp::IObject* parent):
                Component(name[0]=='\0'?Smp::Services::IEventManager::SMP_EventManager:name,descr,parent) {
    for (int i=0;i<=Smp::Services::IEventManager::SMP_PostSimTimeChangeId;++i) {
        std::ostringstream cname;
        cname <<"events"<<i;
        _evRegistry[i]=new Collection<const Smp::IEntryPoint>(cname.str().c_str(),"",this);
    }
}
// ..........................................................
EventManager::~EventManager() {
    for (int i=Smp::Services::IEventManager::SMP_LeaveConnectingId;
            i<=Smp::Services::IEventManager::SMP_PostSimTimeChangeId;++i) {
        delete _evRegistry[i];
    }
}
// --------------------------------------------------------------------
// ..........................................................
void EventManager::Subscribe(Smp::Services::EventId event,
                        const Smp::IEntryPoint* entryPoint) {
    // TODO make it thread safe.
    // TODO check eventId and throw Smp::Services::InvalidEventId if needed.
    Collection<const Smp::IEntryPoint>* eps=_evRegistry[event];
    for (auto ep: *eps) {
        if (ep==entryPoint) {
            // TODO throw Smp::Services::EntryPointAlreadySubscribed
            return;
        }
    }
    eps->push_back(entryPoint);
}
// ..........................................................
void EventManager::Unsubscribe(Smp::Services::EventId event,
                        const Smp::IEntryPoint* entryPoint) {
    // TODO make it thread safe.
    // TODO check eventId and throw Smp::Services::InvalidEventId if needed.
    Collection<const Smp::IEntryPoint>* eps=_evRegistry[event];
    bool res=eps->remove(entryPoint);   
    if (!res) {
        // TODO throw Smp::Services::EntryPointNotSubscribed
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
    for (auto ep: *(_evRegistry[event])) {
        ep->Execute();
    }
}

}} // namespace simph::kern
