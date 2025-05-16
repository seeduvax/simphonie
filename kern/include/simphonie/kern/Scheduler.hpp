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
#include "simdeck/smpext/IObservableScheduler.hpp"
#include "simdeck/smpext/ISchedulerObserver.hpp"
#include "simdeck/smpext/ISchedulerOnEvent.hpp"
#include "simphonie/sys/Synchro.hpp"
#include "simphonie/sys/Thread.hpp"

namespace simphonie {
namespace kern {
using namespace simdeck;

class TimeKeeper;
class Schedule;

class Scheduler : public Component,
                  virtual public simphonie::sys::Runnable,
                  virtual public EntryPointPublisher,
                  virtual public Smp::Services::IScheduler,
                  virtual public smpext::ISchedulerOnEvent,
                  virtual public smpext::IObservableScheduler {
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
    void SetEventStartOnEvent(Smp::Services::EventId eventId, Smp::Services::EventId triggerEventId);
    void SetEventStopOnEvent(Smp::Services::EventId eventId, Smp::Services::EventId triggerEventId);
    void RemoveEvent(Smp::Services::EventId event) override;
    Smp::Services::EventId GetCurrentEventId() const override;
    Smp::Duration GetNextScheduledEventTime() const override;
    Smp::Bool IsEventScheduled(Smp::Services::EventId eventId) const override;

    void RegisterObserver(smpext::ISchedulerObserver* observer) override;
    void RemoveObserver(smpext::ISchedulerObserver* observer) override;
    const smpext::ISchedule* GetSchedule() const override;

    /**
     * Run next schedule event.
     * @warning partly thread safe. Can be called concurrently from other
     * scheduler methods, but shall the step method itself is not fully
     * reentrant.
     */
    void step();

    // Runnable implementation.
    // TODO consider rework sys/thread to use entry point.
    void run() override;

protected:
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
    // TODO check mutable here
    mutable std::mutex _mutex;
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
    void schedule(Schedule* s);
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

    std::vector<smpext::ISchedulerObserver*> _observers;
};

} /* namespace kern */
}  // namespace simphonie
#endif /* __simphonie_kern_Scheduler_HPP__ */
