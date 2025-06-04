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

#include <time.h>
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
    void _init();
    void _updateGoal();

    struct timespec _goal, _period;
    Smp::Duration _periodSmp;
    Smp::UInt64 _overflowCount;
    Smp::Duration _margin;
    Smp::Bool _startSec;
};

} /* namespace colibry */
} /* namespace simphonie */
#endif /* __simphonie_colibry_Synchronization_HPP__ */
