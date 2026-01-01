/*
 * @file EventManager.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_EventManager_HPP__
#define __simphonie_kern_EventManager_HPP__
#include <unordered_map>
#include "Smp/Services/IEventManager.h"
#include "simdeck/Collection.hpp"
#include "simdeck/Component.hpp"
#include "simphonie/sys/Synchro.hpp"

namespace simphonie {
namespace kern {
using namespace simdeck;

/**
 *
 */
class EventManager : public Component, virtual public Smp::Services::IEventManager {
public:
    /**
     * Default constructor.
     */
    EventManager(Smp::String8 name, Smp::String8 descr = "",
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
    typedef std::unordered_map<Smp::Services::EventId, Collection<const Smp::IEntryPoint> > EventRegistryMap;
    EventRegistryMap _evRegistry;
    std::unordered_map<std::string, Smp::Services::EventId> _idIndex;
    Smp::Services::EventId _nextId=1;
    std::mutex _mutex;
    bool _emitting=false;

    struct SubQuery {
    public:
        Smp::Services::EventId _event;
        bool _sub;
        const Smp::IEntryPoint* _ep;
    };
    std::vector<SubQuery> _subQueries;
};

}  // namespace kern
}  // namespace simph
#endif  // __simphonie_kern_EventManager_HPP__
