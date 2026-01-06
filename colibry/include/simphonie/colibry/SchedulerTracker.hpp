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
#include "Smp/Services/IScheduler.h"
#include "simdeck/EntryPointPublisher.hpp"
#include "simdeck/Service.hpp"
#include <map>

namespace simphonie {
namespace colibry {

class SchedulerTracker : public simdeck::Service,
    virtual public simdeck::EntryPointPublisher {
public:
    SchedulerTracker(Smp::String8 name, Smp::String8 descr = "", Smp::IObject *parent = nullptr);
    ~SchedulerTracker() = default;

protected:
    void connect() override;

private:
    struct ExecStat {
        Smp::Duration duration;
        Smp::Duration min;
        Smp::Duration max;
        Smp::Float64 mean;
        uint64_t count;
    };

    void epPreExec();
    void epPostExec();
    void epLogStats();

    Smp::Services::IScheduler* _scheduler;
    Smp::Services::ITimeKeeper* _timeKeeper;
    Smp::Duration _start;
    std::map<Smp::Services::EventId, ExecStat> _stats;
};

} /* namespace colibry */
} /* namespace simphonie */
#endif /* __simphonie_colibry_SchedulerTracker_HPP__ */
