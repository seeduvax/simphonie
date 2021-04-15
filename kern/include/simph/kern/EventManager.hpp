/*
 * @file EventManager.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_EventManager_HPP__
#define __simph_kern_EventManager_HPP__
#include <unordered_map>
#include "Smp/Services/IEventManager.h"
#include "simph/smpdk/Collection.hpp"
#include "simph/smpdk/Component.hpp"

namespace simph {
namespace kern {
using namespace simph::smpdk;

/**
 *
 */
class EventManager : public Component, virtual public Smp::Services::IEventManager {
public:
    /**
     * Default constructor.
     */
    EventManager(Smp::String8 name = Smp::Services::IEventManager::SMP_EventManager, Smp::String8 descr = "",
                 Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~EventManager();

    // Smp::Services::IEventManager implementation
    Smp::Services::EventId QueryEventId(Smp::String8 eventName);
    void Subscribe(Smp::Services::EventId event, const Smp::IEntryPoint* entryPoint);
    void Unsubscribe(Smp::Services::EventId event, const Smp::IEntryPoint* entryPoint);
    void Emit(Smp::Services::EventId event, Smp::Bool syncrhonous = true);

private:
    typedef std::unordered_map<Smp::Services::EventId, Collection<const Smp::IEntryPoint> > EventRegisteryMap;
    EventRegisteryMap _evRegistry;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_EventManager_HPP__
