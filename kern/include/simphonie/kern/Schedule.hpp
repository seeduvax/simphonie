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

class Schedule : public simdeck::Object,
    virtual public simdeck::smpext::ISchedule,
    virtual public simdeck::EntryPointPublisher {
public:
    static constexpr Smp::Services::EventId NO_EVENTID = -1L;

    Schedule(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent, const Smp::IEntryPoint* ep,
             const std::vector<Smp::IOutputField*>& fields, Smp::Duration simTime, Smp::Duration period = 0,
             Smp::Int64 repeat = 0, Smp::UInt64 priority = 0);

    inline Smp::Services::EventId GetId() const override { return _id; }
    inline Smp::Duration GetTime() const override { return _simTime; }
    inline Smp::Duration GetPeriod() const override { return _period; }
    inline Smp::Int64 GetRepeat() const override { return _repeat; }
    inline Smp::Int64 GetPriority() const {
        return _priority;
    } /* TODO add to simdeck::smpext::ISchedule */
    inline Smp::Int64 GetActivationCounter() const override {
        return _counterActivation; }
    inline Smp::Services::EventId GetStartEventId() const override {
        return _startEventId; }
    inline Smp::Services::EventId GetStopEventId() const override {
        return _stopEventId; }
    inline const Smp::IEntryPoint* GetEP() const override {
        return _ep; }
    inline Smp::Bool IsCompleted() const override { return _completed; }

    void setTime(Smp::Duration simTime, Smp::Bool updateScheduler = true);
    inline void setPeriod(Smp::Duration period) { _period = period; }
    inline void setRepeat(Smp::Int64 repeat) { _repeat = repeat; }
    void setPriority(Smp::UInt64 priority, Smp::Bool updateScheduler = true);
    void setStartEventId(Smp::Services::EventId startEventId);
    void setStopEventId(Smp::Services::EventId stopEventId);

    void setIsWaiting(Smp::Bool isWaiting, Smp::Bool updateScheduler = true);

    inline Smp::Bool isWaiting() const { return _isWaiting; }

    void run();

    bool operator<(const Schedule& other) const {
        if (_isWaiting && !other.isWaiting()) {
            return false;
        }
        if (other.isWaiting() && !_isWaiting) {
            return true;
        }
        if (_simTime != other.GetTime()) {
            return _simTime < other.GetTime();
        }
        if (_priority != other.GetPriority()) {
            return _priority > other.GetPriority();
        }
        return _id < other.GetId();
    }

private:
    void _epStartOnEvent();
    inline void _epStopOnEvent() { _scheduler->RemoveEvent(_id); }

    Scheduler* _scheduler;

    const Smp::IEntryPoint* _ep;
    std::vector<Smp::IOutputField*> _fields;
    Smp::Duration _simTime;
    Smp::Duration _period;
    Smp::Int64 _repeat;
    Smp::UInt64 _priority;
    Smp::Services::EventId _id;
    Smp::Bool _completed;
    Smp::Int64 _counterActivation;
    Smp::Services::EventId _startEventId, _stopEventId;
    Smp::Bool _isWaiting;

};

} /* kern */
} /* simphonie */

#endif /* __simphonie_kern_Schedule_HPP__ */
 