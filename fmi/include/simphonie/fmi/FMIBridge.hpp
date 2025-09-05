/*
 * @file FMIBridge.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_fmi_FMIBridge_HPP__
#define __simphonie_fmi_FMIBridge_HPP__

#include <condition_variable>
#include <mutex>
#include <set>

#include "Smp/ISimpleField.h"
#include "Smp/ISimulator.h"
#include "Smp/Services/IScheduler.h"
#include "Smp/Services/ITimeKeeper.h"
#include "cppfmu/cppfmu_cs.hpp"
#include "simdeck/EntryPointPublisher.hpp"
#include "simdeck/Object.hpp"

namespace simphonie {
namespace fmi {

/**
 * @brief Bridge class between Simphonie and the FMI standard
 *
 * It enables to export Simphonie-based simulation as a FMU co-simulation, that
 * can then be loaded by a third-party host simulation that supports such standard.
 */
class FMIBridge : public cppfmu::SlaveInstance, public simdeck::EntryPointPublisher, public simdeck::Object {
public:
    FMIBridge(Smp::ISimulator* sim, Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    ~FMIBridge() = default;

    /**
     * @brief Overrided from `cppfmu::SlaveInstance`
     */
    void SetupExperiment(cppfmu::FMIBoolean toleranceDefined, cppfmu::FMIReal tolerance, cppfmu::FMIReal tStart,
                         cppfmu::FMIBoolean stopTimeDefined, cppfmu::FMIReal tStop) override;
    /**
     * @brief Overrided from `cppfmu::SlaveInstance`
     */

    void Terminate() override;
    /**
     * @brief Overrided from `cppfmu::SlaveInstance`
     */
    void Reset() override;
    /**
     * @brief Overrided from `cppfmu::SlaveInstance`
     */
    void SetReal(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIReal value[]) override;
    /**
     * @brief Overrided from `cppfmu::SlaveInstance`
     */
    void SetInteger(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIInteger value[]) override;
    /**
     * @brief Overrided from `cppfmu::SlaveInstance`
     */
    void SetBoolean(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIBoolean value[]) override;
    /**
     * @brief Overrided from `cppfmu::SlaveInstance`
     */
    void SetString(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIString value[]) override;
    /**
     * @brief Overrided from `cppfmu::SlaveInstance`
     */
    void GetReal(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIReal value[]) const override;
    /**
     * @brief Overrided from `cppfmu::SlaveInstance`
     */
    void GetInteger(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIInteger value[]) const override;
    /**
     * @brief Overrided from `cppfmu::SlaveInstance`
     */
    void GetBoolean(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIBoolean value[]) const override;
    /**
     * @brief Overrided from `cppfmu::SlaveInstance`
     */
    void GetString(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIString value[]) const override;
    /**
     * @brief Overrided from `cppfmu::SlaveInstance`
     */
    bool DoStep(cppfmu::FMIReal currentCommunicationPoint, cppfmu::FMIReal communicationStepSize,
                cppfmu::FMIBoolean newStep, cppfmu::FMIReal& endOfStep) override;

private:
    /**
     * @brief Recursively fill the vector `fields` with the published fields
     *
     * @param c Smp::IComponent*: The parent component
     * @param fields std::vector<Smp::ISimpleField*>&: The vector to be filled with the fields
     */
    static void getAllFields(Smp::IComponent* c, std::vector<Smp::ISimpleField*>& fields);

    /**
     * @brief Compare to fields to sort them
     *
     * Sort them by the lexical order of teir full name (including parents' ones)
     *
     * @param a const Smp::ISimpleField*: First field to be compare
     * @param b const Smp::ISimpleField*: Second field to be compare
     * @return bool: `true` if `a` should be placed before `b`, `false` otherwise
     */
    static bool compareFields(const Smp::ISimpleField* a, const Smp::ISimpleField* b);

    /* EPs */
    void onLeaveExecuting();
    void onSimTimeChanged();

    /* Wrappers for Set* and Get* from cppfmu::SlaveInstance */
    template <typename T>
    void SetGeneric(const cppfmu::FMIValueReference vr[], std::size_t nvr, T value[]);
    template <typename T>
    void GetGeneric(const cppfmu::FMIValueReference vr[], std::size_t nvr, T value[]) const;

    Smp::ISimulator* _sim;
    Smp::Services::IScheduler* _sched;
    Smp::Services::ITimeKeeper* _tk;

    /* For waiting for the leave executing event */
    bool _completed;
    std::mutex _mutex;
    std::condition_variable _monitor;

    /**
     * @brief Absolute simulation time for hlding the simulation. Updated for each `FMIBridge::DoStep` call.
     */
    Smp::Duration _endAbsoluteSimTime;

    /**
     * @brief Mapping from FmiValueReference unsigned int to Smp::ISimpleField*
     */
    std::vector<Smp::ISimpleField*> _fmiRef2Field;
};

} /* namespace fmi */
} /* namespace simphonie */
#endif /* __simphonie_fmi_FMIBridge_HPP__ */
