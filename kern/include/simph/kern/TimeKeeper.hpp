/*
 * @file TimeKeeper.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_TimeKeeper_HPP__
#define __simph_kern_TimeKeeper_HPP__
#include "Smp/Services/ITimeKeeper.h"
#include "simph/kern/Component.hpp"

namespace simph {
	namespace kern {

/**
 *
 */
class TimeKeeper: virtual public Component,
                  virtual public Smp::Services::ITimeKeeper {
public:
    /**
     * Default constructor.
     */
    TimeKeeper(Smp::String8 name=Smp::Services::ITimeKeeper::SMP_TimeKeeper,
                     Smp::String8 descr="",
                     Smp::IObject* parent=nullptr);
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
    
    // TODO should not be public but binded to some simulation start even handling
    void reset();
private:
    Smp::Duration _simTime;
    Smp::Duration _epochOffset;
    Smp::DateTime _missionStart;
    static Smp::DateTime _y2kJan1Offset;

};

}} // namespace simph::kern
#endif // __simph_kern_TimeKeeper_HPP__