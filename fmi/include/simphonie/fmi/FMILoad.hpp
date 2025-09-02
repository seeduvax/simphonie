/*
 * @file FMILoad.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_fmi_FMILoad_HPP__
#define __simphonie_fmi_FMILoad_HPP__

#include <functional>

#include "Smp/IService.h"
#include "Smp/Services/ILogger.h"
#include "Smp/Services/ITimeKeeper.h"
#include "cppfmu/fmi2FunctionTypes.h"
#include "cppfmu/fmi2TypesPlatform.h"
#include "simdeck/Component.hpp"
#include "simdeck/EntryPointPublisher.hpp"
#include "simphonie/sys/DLib.hpp"

namespace simphonie {
namespace fmi {

/**
 * @brief Class for handling an external FMI compliant simulation
 *
 * State machine of the FMI v2.0 standard fro co-simulation  can be found at
 * https://fmi-standard.org/assets/releases/FMI_for_ModelExchange_and_CoSimulation_v2.0.pdf
 * page 103.
 *
 * @warning The full fmi standard is not yet implemented, especially derivatives and FMU State
 */
class FMILoad : public simdeck::Component, virtual public Smp::IService, virtual public simdeck::EntryPointPublisher {
public:
    FMILoad(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    ~FMILoad();

    void terminate();
    void reset();
    bool doStep(fmi2Real stepSize);

private:
    void publish(Smp::IPublication* receiver) override;
    void configure() override;
    void setup();
    inline void doStepEP() {
        doStep(_stepSize);
    };
    static void loggingFn(fmi2ComponentEnvironment componentEnvironment, fmi2String instanceName, fmi2Status status,
                          fmi2String category, fmi2String message, ...);
    static void stepFinished(fmi2ComponentEnvironment componentEnvironment, fmi2Status status){};

    /* published fields */
    std::string _path;
    std::string _instanceName;
    fmi2Real _tolerance;
    fmi2Real _startTime;
    fmi2Real _stopTime;
    fmi2Real _stepSize;

    Smp::Services::ILogger* _logger;
    Smp::Services::ITimeKeeper* _tk;
    std::unique_ptr<simphonie::sys::DLib> _binary;
    fmi2Component _fmiObj;
    fmi2Real _curComTime;
    fmi2DoStepTYPE* _doStepFn;

    /* arguments to provide to the fmi co-simulation */
    struct cbArgs {
        FMILoad* self;
        Smp::Services::ILogger* logger;
    } _cbArgs;
    fmi2CallbackFunctions _cbFunctions{&loggingFn, &calloc, &free, &stepFinished, &_cbArgs};
};

} /* namespace fmi */
} /* namespace simphonie */
#endif /* __simphonie_fmi_FMILoad_HPP__ */
