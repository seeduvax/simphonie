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
#include "simdeck/EntryPointPublisher.hpp"
#include "simdeck/Object.hpp"
#include "fmi2Functions.h"

namespace simphonie {
namespace fmi {

/**
 * @brief Bridge class between Simphonie and the FMI standard
 *
 * It enables to export Simphonie-based simulation as a FMU co-simulation, that
 * can then be loaded by a third-party host simulation that supports such standard.
 */
class FMIBridge : public simdeck::EntryPointPublisher, public simdeck::Object {
public:
    FMIBridge(Smp::ISimulator* sim, Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    ~FMIBridge() = default;

    /**
     * @brief Overrided from `SlaveInstance`
     */
    void SetupExperiment(fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real tStart,
                         fmi2Boolean stopTimeDefined, fmi2Real tStop);
    /**
     * @brief Overrided from `SlaveInstance`
     */

    void Terminate();
    /**
     * @brief Overrided from `SlaveInstance`
     */
    void Reset();
    /**
     * @brief Overrided from `SlaveInstance`
     */
    void SetReal(const fmi2ValueReference vr[], std::size_t nvr, const fmi2Real value[]);
    /**
     * @brief Overrided from `SlaveInstance`
     */
    void SetInteger(const fmi2ValueReference vr[], std::size_t nvr, const fmi2Integer value[]);
    /**
     * @brief Overrided from `SlaveInstance`
     */
    void SetBoolean(const fmi2ValueReference vr[], std::size_t nvr, const fmi2Boolean value[]);
    /**
     * @brief Overrided from `SlaveInstance`
     */
    void SetString(const fmi2ValueReference vr[], std::size_t nvr, const fmi2String value[]);
    /**
     * @brief Overrided from `SlaveInstance`
     */
    void GetReal(const fmi2ValueReference vr[], std::size_t nvr, fmi2Real value[]) const;
    /**
     * @brief Overrided from `SlaveInstance`
     */
    void GetInteger(const fmi2ValueReference vr[], std::size_t nvr, fmi2Integer value[]) const;
    /**
     * @brief Overrided from `SlaveInstance`
     */
    void GetBoolean(const fmi2ValueReference vr[], std::size_t nvr, fmi2Boolean value[]) const;
    /**
     * @brief Overrided from `SlaveInstance`
     */
    void GetString(const fmi2ValueReference vr[], std::size_t nvr, fmi2String value[]) const;
    /**
     * @brief Overrided from `SlaveInstance`
     */
    bool DoStep(fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize,
                fmi2Boolean newStep, fmi2Real& endOfStep);

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

    /* Wrappers for Set* and Get* from SlaveInstance */
    template <typename T>
    void SetGeneric(const fmi2ValueReference vr[], std::size_t nvr, T value[]);
    template <typename T>
    void GetGeneric(const fmi2ValueReference vr[], std::size_t nvr, T value[]) const;

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
