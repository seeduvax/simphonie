


Scheduler events and thread policy.

Emitted events:
    ScheduledEventCompleted: when a schedule event has been run
    SchedulerCompleted: when there the scheduler ends by itself when there is no more event in the schedule queue.

Subscrived events:
    SMP_EnterExecuting: starts scheduler thread (if not yet running).
    SMP_LeaveExecuting: stop the scheduler thread (if running), do not interrupt current event, stop before fetching next event.

TBC:
    What GetNextScheduledEventTime() shall return when schedule queue is empty()? 0 or MAX_DURATION?

Interaction with other services:
    TimeKeeper shall subscribe to ScheduledEventCompleted to update simulation time according next simulation event if any.

    SimulationCompletionService :
        Subscribe ScheduledEventCompleted: request simulator Hold when stop condition are met.
        Subscribe Smp_PostSimTimeChange: request simulator Hold when max simulation time is reached.
        Subscribe SchedulerCompleted: request simulator Hold when no more events in the scheduler.
            TBC: do not do that, check GetNextEventSimulationTime on ScheduledEventCompleted to fire Hold.



