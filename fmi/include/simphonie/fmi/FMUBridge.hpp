/*
 * @file FMUBridge.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_fmi_FMUBridge_HPP__
#define __simphonie_fmi_FMUBridge_HPP__

#include <condition_variable>
#include <mutex>

#include "Smp/ISimulator.h"
#include "Smp/Services/IScheduler.h"
#include "Smp/Services/ITimeKeeper.h"
#include "cppfmu/cppfmu_cs.hpp"
#include "simdeck/EntryPointPublisher.hpp"
#include "simdeck/Object.hpp"

namespace simphonie {
namespace fmi {

class FMUBridge : public cppfmu::SlaveInstance, public simdeck::EntryPointPublisher, public simdeck::Object {
public:
    FMUBridge(Smp::ISimulator* sim, Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    virtual ~FMUBridge() = default;

    void SetupExperiment(cppfmu::FMIBoolean toleranceDefined, cppfmu::FMIReal tolerance, cppfmu::FMIReal tStart,
                         cppfmu::FMIBoolean stopTimeDefined, cppfmu::FMIReal tStop) override;
    void Terminate() override;
    void Reset() override;
    void SetReal(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIReal value[]) override;
    void SetInteger(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIInteger value[]) override;
    void SetBoolean(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIBoolean value[]) override;
    void SetString(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIString value[]) override;
    void GetReal(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIReal value[]) const override;
    void GetInteger(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIInteger value[]) const override;
    void GetBoolean(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIBoolean value[]) const override;
    void GetString(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIString value[]) const override;
    bool DoStep(cppfmu::FMIReal currentCommunicationPoint, cppfmu::FMIReal communicationStepSize,
                cppfmu::FMIBoolean newStep, cppfmu::FMIReal& endOfStep) override;

private:
    void hold();
    void onLeaveExecuting();

    // std::unordered_map<const cppfmu::FMIValueReference, Smp::String8> _fmiRef2Str;
    Smp::ISimulator* _sim;
    Smp::Services::IScheduler* _sched;
    Smp::Services::ITimeKeeper* _tk;
    Smp::IEntryPoint* _holdEP;
    bool _completed = false;
    std::mutex _mutex;
    std::condition_variable _monitor;
};

} /* namespace fmi */
} /* namespace simphonie */
#endif /* __simphonie_fmi_FMUBridge_HPP__ */
