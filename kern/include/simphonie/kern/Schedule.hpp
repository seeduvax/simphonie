/*
 * @file Schedule.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_Schedule_HPP__
#define __simphonie_kern_Schedule_HPP__

#include "Smp/IObject.h"
#include "Smp/IEntryPoint.h"
#include "Smp/Duration.h"
#include "Smp/IOutputField.h"
#include "Smp/Int64.h"
#include "Smp/Services/EventId.h"
#include "Smp/ISimulator.h"
#include "simdeck/Object.hpp"
#include "simdeck/smpext/ISchedule.hpp"
#include "simdeck/ExInvalidParent.hpp"
#include "simdeck/EntryPointPublisher.hpp"
#include "simphonie/kern/Scheduler.hpp"
#include <vector>
#include <atomic>

namespace simphonie {
namespace kern {

class Schedule {
public:
    Schedule(Scheduler* scheduler, const Smp::IEntryPoint* ep, const std::vector<Smp::IOutputField*>& fields,
             Smp::Duration simTime, Smp::Duration period = 0, Smp::Int64 repeat = 0);

    inline Smp::Services::EventId GetId() const {
        return _id;
    }
    inline Smp::Duration GetTime() const {
        return _absoluteSimTime;
    }
    inline Smp::Duration GetPeriod() const {
        return _period;
    }
    inline Smp::Int64 GetRepeat() const {
        return _repeat;
    }
    inline Smp::Int64 GetActivationCounter() const {
        return _counterActivation;
    }
    inline const Smp::IEntryPoint* GetEP() const {
        return _ep;
    }
    inline Smp::Bool IsCompleted() const {
        return _completed;
    }

    void setTime(Smp::Duration absoluteSimTime);
    inline void setPeriod(Smp::Duration period) { _period = period; }
    inline void setRepeat(Smp::Int64 repeat) { _repeat = repeat; }

    void run();

    bool operator<(const Schedule& other) const {
        if (_absoluteSimTime != other.GetTime()) {
            return _absoluteSimTime < other.GetTime();
        }
        return _id < other.GetId();
    }

private:
    void _epStartOnEvent();
    inline void _epStopOnEvent() { _scheduler->RemoveEvent(_id); }

    Scheduler* _scheduler;

    const Smp::IEntryPoint* _ep;
    std::vector<Smp::IOutputField*> _fields;
    Smp::Duration _absoluteSimTime;
    Smp::Duration _period;
    Smp::Int64 _repeat;
    Smp::Services::EventId _id;
    Smp::Bool _completed;
    Smp::Int64 _counterActivation;
};

} /* kern */
} /* simphonie */

#endif /* __simphonie_kern_Schedule_HPP__ */
