/*
 * @file Synchronizer.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_colibry_Synchronization_HPP__
#define __simphonie_colibry_Synchronization_HPP__

#include <condition_variable>
#include <mutex>
#include "Smp/IPublication.h"
#include "Smp/IService.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simdeck/Component.hpp"
#include "simdeck/EntryPointPublisher.hpp"

namespace simphonie {
namespace colibry {

class Synchronizer : public simdeck::Component,
                     virtual public simdeck::EntryPointPublisher,
                     virtual public Smp::IService {
public:
    Synchronizer(Smp::String8 name, Smp::String8 description = "", Smp::IObject* parent = nullptr);
    ~Synchronizer() = default;

    void step();

protected:
    void connect() override;
    void publish(Smp::IPublication* receiver) override;

private:
    /* Clock and duration used by the Synchronizer class */
    typedef std::chrono::steady_clock _Clock;
    typedef std::chrono::nanoseconds _Duration;

    void _init();

    std::chrono::time_point<_Clock, _Duration> _goal;
    _Duration _period;
    Smp::Duration _periodSmp;
    Smp::UInt64 _overflowCount;
    Smp::Duration _margin;
};

} /* namespace colibry */
} /* namespace simphonie */
#endif /* __simphonie_colibry_Synchronization_HPP__ */
