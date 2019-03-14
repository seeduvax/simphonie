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
#include <queue>
#include <unordered_map>

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
     * Registery of schedule objects by event id.
     * Shared references to support nested scheduling call.
     * TODO discuss the choice of implementation
     */
    typedef std::shared_ptr<Schedule> SchedulePtr;
    typedef std::unordered_map<Smp::Services::EventId, SchedulePtr > ScheduledMap;
    ScheduledMap _scheduled;

    /*
     * Enqueue and sort schedule objects according to the schedule time in this.
     * Stored weak schedule references support nested call of RemoveEvent.
     * TODO discuss the choice of implementation
     */
    typedef std::pair< Smp::Duration, std::weak_ptr<Schedule> > ScheduledQueueData;
    typedef std::priority_queue< ScheduledQueueData, std::vector< ScheduledQueueData >, bool (*) (const ScheduledQueueData&, const ScheduledQueueData&) > ScheduledQueue;
    ScheduledQueue _scheduledQueue;

    SchedulePtr _currentSchedule;

    void pushToQueue(const SchedulePtr& schedule);

    SchedulePtr findSchedule(Smp::Services::EventId event) const;

    static bool compareSchedule(const ScheduledQueueData& a, const ScheduledQueueData& b);
};

}} // namespace simph::kern
#endif // __simph_kern_Scheduler_HPP__
