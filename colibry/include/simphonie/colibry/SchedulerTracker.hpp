/*
 * @file SchedulerTracker.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_colibry_SchedulerTracker_HPP__
#define __simphonie_colibry_SchedulerTracker_HPP__

#include "Smp/String8.h"
#include "Smp/Services/EventId.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simdeck/EntryPointPublisher.hpp"
#include "simdeck/Service.hpp"
#include "simdeck/smpext/IObservableScheduler.hpp"
#include <unordered_map>

namespace simphonie {
namespace colibry {

class SchedulerTracker : public simdeck::Service,
    virtual public simdeck::EntryPointPublisher {
public:
    SchedulerTracker(Smp::String8 name, Smp::String8 descr = "", Smp::IObject *parent = nullptr);
    ~SchedulerTracker() = default;

protected:
    void connect();

private:
    struct _Event {
        struct Call {
            const Smp::Duration start;
            Smp::Duration stop;
        };

        const Smp::Services::EventId id;
        const std::string name;
        std::vector<Call> calls;
    };

    void _preEventEP();
    void _postEventEP();
    void _logStats();

    simdeck::smpext::IObservableScheduler *_scheduler;
    Smp::Services::ITimeKeeper *_timeKeeper;
    _Event *_currentEvent;
    std::unordered_map<Smp::Services::EventId, _Event> _events;

};

} /* namespace colibry */
} /* namespace simphonie */
#endif /* __simphonie_colibry_SchedulerTracker_HPP__ */
