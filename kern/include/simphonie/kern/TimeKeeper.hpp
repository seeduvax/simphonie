/*
 * @file TimeKeeper.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_TimeKeeper_HPP__
#define __simphonie_kern_TimeKeeper_HPP__
#include "Smp/Services/IEventManager.h"
#include "Smp/Services/ITimeKeeper.h"
#include "Smp/Services/IScheduler.h"
#include "simdeck/Component.hpp"
#include "simdeck/EntryPointPublisher.hpp"

namespace simphonie {
namespace kern {
using namespace simdeck;

/**
 *
 */
class TimeKeeper : virtual public Component, 
                   virtual public EntryPointPublisher,
                   virtual public Smp::Services::ITimeKeeper {
public:
    /**
     * Default constructor.
     */
    TimeKeeper(Smp::String8 name, Smp::String8 descr = "",
               Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~TimeKeeper();
    // Smp::Services::ITimeKeeper implementation
    Smp::Duration GetSimulationTime() const;
    Smp::DateTime GetEpochTime() const;
    Smp::DateTime GetMissionStartTime() const;
    Smp::Duration GetMissionTime() const;
    Smp::DateTime GetZuluTime() const;
    void SetSimulationTime(Smp::Duration simulationTime);
    void SetEpochTime(Smp::DateTime epochTime);
    void SetMissionStartTime(Smp::DateTime missionStart);
    void SetMissionTime(Smp::Duration missionTime);

    // from SMP header: MissionTime=EpochTime - MissionStart;
    // => SimTime = EpochTime - EpochOffset, where EpochOffset is EpochTime at
    //    simulation start.

protected:
    void connect();
    void publish(Smp::IPublication* receiver);

private:
    void epUpdateSimulationTime();

    Smp::Duration _simTime=0;
    Smp::Duration _nextEventTime=0;
    Smp::Duration _epochOffset=0;
    Smp::DateTime _missionStart=0;
    Smp::Services::IEventManager* _eventMgr=nullptr;
    Smp::Services::IScheduler* _scheduler=nullptr;
    bool _inPreSimTimeChange=false;
    static Smp::DateTime _y2kJan1Offset;
};

}  // namespace kern
}  // namespace simph
#endif  // __simphonie_kern_TimeKeeper_HPP__
