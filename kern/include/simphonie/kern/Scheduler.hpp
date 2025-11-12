/*
 * @file Scheduler.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_Scheduler_HPP__
#define __simphonie_kern_Scheduler_HPP__

#include <set>
#include "Smp/Services/IEventManager.h"
#include "Smp/Services/IScheduler.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simdeck/Component.hpp"
#include "simdeck/EntryPointPublisher.hpp"

/* TODO: about what's commented and related to IObservableScheduler:
 * Design thinking still in progress. Scheduler observability still under study
 * and actual implementation is removed for now.
#include "simdeck/smpext/IObservableScheduler.hpp"
#include "simdeck/smpext/ISchedulerObserver.hpp"
 */
#include "simphonie/sys/Synchro.hpp"
#include "simphonie/sys/Thread.hpp"

namespace simphonie {
namespace kern {
using namespace simdeck;

class TimeKeeper;
class Schedule;

#define DURATION_MAX INT64_MAX

class Scheduler : public Component,
                  virtual public simphonie::sys::Runnable,
                  virtual public EntryPointPublisher,
                  virtual public Smp::Services::IScheduler  //,
/* virtual public smpext::IObservableScheduler */ {
public:
    Scheduler(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    virtual ~Scheduler();

    Smp::Services::EventId AddImmediateEvent(const Smp::IEntryPoint* entryPoint) override;
    Smp::Services::EventId AddSimulationTimeEvent(const Smp::IEntryPoint* entryPoint, Smp::Duration simulationTime,
                                                  Smp::Duration cycleTime = 0, Smp::Int64 repeat = 0) override;
    Smp::Services::EventId AddMissionTimeEvent(const Smp::IEntryPoint* entryPoint, Smp::Duration missionTime,
                                               Smp::Duration cycleTime = 0, Smp::Int64 repeat = 0) override;
    Smp::Services::EventId AddEpochTimeEvent(const Smp::IEntryPoint* entryPoint, Smp::DateTime epochTime,
                                             Smp::Duration cycleTime = 0, Smp::Int64 repeat = 0) override;
    Smp::Services::EventId AddZuluTimeEvent(const Smp::IEntryPoint* entryPoint, Smp::DateTime zuluTime,
                                            Smp::Duration cycleTime = 0, Smp::Int64 repeat = 0) override;
    Smp::Services::EventId AddRelativeZuluTimeEvent(const Smp::IEntryPoint* entryPoint, Smp::DateTime zuluTimeDelay,
                                                    Smp::Duration cycleTime = 0, Smp::Int64 repeat = 0) override;

    void SetEventSimulationTime(Smp::Services::EventId event, Smp::Duration simulationTime) override;
    void SetEventMissionTime(Smp::Services::EventId event, Smp::Duration missionTime) override;
    void SetEventEpochTime(Smp::Services::EventId event, Smp::DateTime epochTime) override;
    void SetEventZuluTime(Smp::Services::EventId eventId, Smp::DateTime zuluTime) override;
    void SetEventCycleTime(Smp::Services::EventId event, Smp::Duration cycleTime) override;
    void SetEventRepeat(Smp::Services::EventId event, Smp::Int64 repeat) override;
    void RemoveEvent(Smp::Services::EventId event) override;
    Smp::Services::EventId GetCurrentEventId() const override;
    Smp::Duration GetNextScheduledEventTime() const override;
    Smp::Bool IsEventScheduled(Smp::Services::EventId eventId) const override;

    /* TODO to be reconsidered
        void RegisterObserver(smpext::ISchedulerObserver* observer) override;
        void RemoveObserver(smpext::ISchedulerObserver* observer) override;
        const smpext::ISchedule* GetSchedule() const override;
     */

    /**
     * Run next schedule event.
     * @warning partly thread safe. Can be called concurrently from other
     * scheduler methods, but shall the step method itself is not fully
     * reentrant.
     */
    void step();

    // Runnable implementation.
    void run() override;

protected:
    void publish(Smp::IPublication* receiver);
    void connect() override;

    Smp::Services::EventId schedule(const Smp::IEntryPoint* entryPoint, Smp::Duration absoluteSimTime,
                                    Smp::Duration cycleTime = 0, Smp::Int64 repeat = 0);
    void schedule(Smp::Services::EventId event, Smp::Duration absoluteSimTime);

    void updateSchedule(Smp::Services::EventId eventId);
    friend Schedule;

private:
    struct _compareSchedule {
        bool operator()(const Schedule* a, const Schedule* b) const;
    };
    typedef std::multiset<Schedule*, _compareSchedule> ScheduledQueue;

    Smp::Services::ITimeKeeper* _timeKeeper;
    Smp::Services::IEventManager* _eventMgr;
    Smp::Services::EventId _preEventExecuteId;
    Smp::Services::EventId _postEventExecuteId;
    bool _run;
    mutable std::mutex _mutex; /* mutable keryword use to be able to modify the mutex from const functions too */
    std::condition_variable _monitor;
    std::unique_ptr<simphonie::sys::Thread> _th;

    /*
     * No more used shared_ptr/weak_ptr but just reguular pointer since the
     * Schedule pointers are not shared and used only internally by the
     * Scheduler where it iis quite easy to master life span of each Schedule
     * instance. Then the overhead of the smart pointer (ref count management)
     * has finally very low added value.
     */
    ScheduledQueue _scheduled;
    Schedule* _currentSchedule;
    Smp::Int64 _activableCount;
    /**
     * search for a schedule by evnet id.
     * This method also optionnaly remove the ound schedule to avoid having
     * to iterate the _scheduled once again when the found event is to be
     * removed.cd lu
     * @param event event id.
     * @param remove
     *   - false, just return the schedule.
     *   - true, the found schedule is removed from _scheduled.
     * @return 1st schedule in scheduled having matching id or nullptr if not
     * any.
     */
    Schedule* findSchedule(Smp::Services::EventId event, bool remove = false);
    void schedule(Schedule* s, bool newSchedule = true);
    // For internal use when mutex is already owned.
    inline Smp::Duration getNextScheduledEventTime() const;
    /**
     * SMP Enter Executing event entry point.
     * Starts the scheduler threads if not yet running.
     */ 
    void epEnterExecuting();
    Smp::IEntryPoint* _epEnterExecuting;
    /**
     * SMP Leave Executing event entry point.
     * If running, request stop, and join the scheduler thread.
     */
    void epLeaveExecuting();
    Smp::IEntryPoint* _epLeaveExecuting;

    /* TODO to be reconsidered
        std::vector<smpext::ISchedulerObserver*> _observers;
    */
    inline Smp::Duration getAbsoluteTime(Smp::Duration relativeTime) {
        // saturate absolute simulation time to avoid overflow and possibly
        // negative resulting value.
        return relativeTime >= (DURATION_MAX - _timeKeeper->GetSimulationTime())
                   ? DURATION_MAX
                   : _timeKeeper->GetSimulationTime() + relativeTime;
    }
};

} /* namespace kern */
}  // namespace simphonie
#endif /* __simphonie_kern_Scheduler_HPP__ */
