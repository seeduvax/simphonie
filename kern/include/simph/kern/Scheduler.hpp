/*
 * @file Scheduler.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Scheduler_HPP__
#define __simph_kern_Scheduler_HPP__

#include "Smp/Services/ITimeKeeper.h"
#include "Smp/Services/IScheduler.h"
#include "simph/kern/Component.hpp"
#include "simph/sys/Synchro.hpp"
#include "simph/sys/Thread.hpp"
#include <set>

namespace simph {
	namespace kern {
class Schedule;
/**
 *
 */
class Scheduler: virtual public Component, 
                 virtual public simph::sys::Runnable,
                 virtual public Smp::Services::IScheduler {
public:
    /**
     * Default constructor.
     */
    Scheduler(Smp::String8 name=Smp::Services::IScheduler::SMP_Scheduler,
                Smp::String8 descr="",
                Smp::IObject* parent=nullptr);
    /**
     * Destructor.
     */
    virtual ~Scheduler();

    // Smp::Services//IScheduler implementation
    Smp::Services::EventId AddImmediateEvent(
                                const Smp::IEntryPoint* entryPoint);
    Smp::Services::EventId AddSimulationTimeEvent(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::Duration simulationTime,
                                Smp::Duration cycleTime=0,
                                Smp::Int64 repeat=0);
    Smp::Services::EventId AddMissionTimeEvent(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::Duration missionTime,
                                Smp::Duration cycleTime=0,
                                Smp::Int64 repeat=0);
    Smp::Services::EventId AddEpochTimeEvent(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::DateTime epochTime,
                                Smp::Duration cycleTime=0,
                                Smp::Int64 repeat=0);
    Smp::Services::EventId AddZuluTimeEvent(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::DateTime zuluTime,
                                Smp::Duration cycleTime=0,
                                Smp::Int64 repeat=0);
    void SetEventSimulationTime(Smp::Services::EventId event,
                                Smp::Duration simulationTime);
    void SetEventMissionTime(Smp::Services::EventId event,
                            Smp::Duration missionTime);
    void SetEventEpochTime(Smp::Services::EventId event,
                            Smp::DateTime epochTime);
    void SetEventZuluTime(Smp::Services::EventId eventId,
                            Smp::DateTime zuluTime);
    void SetEventCycleTime(Smp::Services::EventId event,
                            Smp::Duration cycleTime);
    void SetEventRepeat(Smp::Services::EventId event,
                            Smp::Int64  repeat);
    void RemoveEvent(Smp::Services::EventId event);
    Smp::Services::EventId GetCurrentEventId() const;
    Smp::Duration GetNextScheduledEventTime() const;

    void step();
    void step(Smp::Duration duration);
    void run();
    void schedule(Schedule* schedule);
    void start();
    void stop();
protected:
    // Component specialization
    void connect();
    Smp::Services::EventId schedule(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::Duration absoluteSimTime,
                                Smp::Duration cycleTime=0,
                                Smp::Int64 repeat=0);
    void schedule(Smp::Services::EventId event,
                  Smp::Duration absoluteSimTime);
    Schedule* findSchedule(Smp::Services::EventId event, bool erase=false);
private:
    std::multiset<Schedule*,bool (*)(Schedule*,Schedule*)> _scheduled;
    Smp::Services::ITimeKeeper* _timeKeeper;
    Schedule* _currentSchedule;
    bool _autoStop;
    Smp::Duration _stopSimTime;
    bool _run;
    std::mutex _mutex;
    simph::sys::Thread* _th;
};

}} // namespace simph::kern
#endif // __simph_kern_Scheduler_HPP__
