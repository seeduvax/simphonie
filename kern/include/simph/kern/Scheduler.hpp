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
/**
 *
 */
class Scheduler: public Component,
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
                                const Smp::IEntryPoint* entryPoint) override;
    Smp::Services::EventId AddSimulationTimeEvent(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::Duration simulationTime,
                                Smp::Duration cycleTime=0,
                                Smp::Int64 repeat=0) override;
    Smp::Services::EventId AddMissionTimeEvent(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::Duration missionTime,
                                Smp::Duration cycleTime=0,
                                Smp::Int64 repeat=0) override;
    Smp::Services::EventId AddEpochTimeEvent(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::DateTime epochTime,
                                Smp::Duration cycleTime=0,
                                Smp::Int64 repeat=0) override;
    Smp::Services::EventId AddZuluTimeEvent(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::DateTime zuluTime,
                                Smp::Duration cycleTime=0,
                                Smp::Int64 repeat=0) override;
    void SetEventSimulationTime(Smp::Services::EventId event,
                                Smp::Duration simulationTime) override;
    void SetEventMissionTime(Smp::Services::EventId event,
                            Smp::Duration missionTime) override;
    void SetEventEpochTime(Smp::Services::EventId event,
                            Smp::DateTime epochTime) override;
    void SetEventZuluTime(Smp::Services::EventId eventId,
                            Smp::DateTime zuluTime) override;
    void SetEventCycleTime(Smp::Services::EventId event,
                            Smp::Duration cycleTime) override;
    void SetEventRepeat(Smp::Services::EventId event,
                            Smp::Int64  repeat) override;
    void RemoveEvent(Smp::Services::EventId event) override;
    Smp::Services::EventId GetCurrentEventId() const override;
    Smp::Duration GetNextScheduledEventTime() const override;
    /**
     * Run next schedule event.
     * @warning partly thread safe. Can be called concurrently from other
     * scheduler methods, but shall the step method itself is not fully 
     * reentrant.
     */  
    void step();
    void step(Smp::Duration duration);
    void run() override;
    void start();
    void stop();
protected:
    // Component specialization
    void connect() override;

    Smp::Services::EventId schedule(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::Duration absoluteSimTime,
                                Smp::Duration cycleTime=0,
                                Smp::Int64 repeat=0);
    void schedule(Smp::Services::EventId event,
                  Smp::Duration absoluteSimTime);

private:
    Smp::Services::ITimeKeeper* _timeKeeper;
    bool _autoStop;
    Smp::Duration _stopSimTime;
    bool _run;
    // TODO check mutable here
    mutable std::mutex _mutex;
    std::unique_ptr<simph::sys::Thread> _th;

    /*
     * Internal schedule implementation 
     * as a wrapper of entry point
     */
    class Schedule;

    /*
     * No more used shared_ptr/weak_ptr but just reguular pointer since the
     * Schedule pointers are not shared and used only internally by the 
     * Scheduler where it iis quite easy to master life span of each Schedule
     * instance. Then the overhead of the smart pointer (ref count management)
     * has finally very low added value.
     */
    static bool compareSchedule(const Schedule* a, const Schedule* b);
    typedef std::multiset< Schedule*, decltype(compareSchedule)*> ScheduledQueue;
    ScheduledQueue _scheduled;
    Schedule* _currentSchedule;
    /**
     * search for a schedule by evnet id.
     * This method also optionnaly remove the ound schedule to avoid having
     * to iterate the _scheduled once again when the found event is to be
     * removed.
     * @param event event id.
     * @param remove 
     *   - false, just return the schedule.
     *   - true, the found schedule is removed from _scheduled.
     * @return 1st schedule in scheduled having matching id or nullptr if not
     * any.
     */ 
    Schedule* findSchedule(Smp::Services::EventId event,bool remove=false);
    void schedule(Schedule* s);
};

}} // namespace simph::kern
#endif // __simph_kern_Scheduler_HPP__
