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
#include "Smp/IOutputField.h"
#include "simphonie/kern/ExInvalidSimulationTime.hpp"

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
private:
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
            if ( GetTime() != other.GetTime()) {
                return GetTime()!=-1 && GetTime() < other.GetTime()
                      || other.GetTime()==-1; 
                                              // -1 means somehow far away after
                                              // simulation end, that is do not run.
            }
            return GetId() < other.GetId();
        }
        inline void setId() {
            if (_id==-1) {
                _id=_scheduler->_nextId++;
            }
        }

    private:
        Scheduler* _scheduler;
        const Smp::IEntryPoint* _ep;
        std::vector<Smp::IOutputField*> _fields;
        Smp::Duration _absoluteSimTime;
        Smp::Duration _period;
        Smp::Int64 _repeat;
        Smp::Services::EventId _id;
        Smp::Bool _completed;
    };
    struct _compareSchedule {
        bool operator()(const Schedule* a, const Schedule* b) const;
    };
    typedef std::multiset<Schedule*, _compareSchedule> ScheduledQueue;

    Smp::Services::ITimeKeeper* _timeKeeper;
    Smp::Services::IEventManager* _eventMgr;
    Smp::Services::EventId _preEventExecuteId;
    Smp::Services::EventId _postEventExecuteId;
    Smp::Duration _simulationTime=0;
    bool _run=false;
    bool _autoStop=true;
    mutable std::mutex _mutex; /* mutable keryword use to be able to modify the mutex from const functions too */
    std::condition_variable _monitor;
    std::unique_ptr<simphonie::sys::Thread> _th;
    Smp::Services::EventId _nextId=0;

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
     * @return 1st schedule in scheduled having matching id or nullptr if not
     * any.
     */
    Schedule* findSchedule(Smp::Services::EventId event);
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

    /**
     * check relative time and compute absolute simulation time.
     * Throws invalid simulation time exception when relative time is too
     * big against current simulation time, leading to a simulation time 
     * overflow.
     * @param relativeTime relative time.
     * @return absolute simulation time.
     */  
    Smp::Duration getAbsoluteTime(Smp::Duration relativeTime);
};

} /* namespace kern */
}  // namespace simphonie
#endif /* __simphonie_kern_Scheduler_HPP__ */
