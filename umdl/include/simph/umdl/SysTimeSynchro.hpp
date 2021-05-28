/*
 * @file SysTimeSynchro.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_umdl_SysTimeSynchro_HPP__
#define __simph_umdl_SysTimeSynchro_HPP__
#include <chrono>
#include "simph/kern/AStepMdl.hpp"
#include "simph/sys/Synchro.hpp"
namespace simph {
namespace umdl {

/**
 *
 */
class SysTimeSynchro : public simph::kern::AStepMdl {
public:
    /**
     * Default constructor.
     */
    SysTimeSynchro(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~SysTimeSynchro();
    // AStepMdl implementation
    void step();

protected:
    void publish(Smp::IPublication* receiver);

private:
    Smp::Duration _period;
    std::chrono::time_point<std::chrono::system_clock> _nextTick;
    std::mutex _mutex;
    std::condition_variable _monitor;
};

}  // namespace umdl
}  // namespace simph
#endif  // __simph_umdl_SysTimeSynchro_HPP__
