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

#include "Smp/ISimulator.h"
#include "cppfmu/cppfmu_cs.hpp"
#include "simdeck/EntryPointPublisher.hpp"
#include "simdeck/Object.hpp"
#include "wfrest/Json.h"

namespace simphonie {
namespace fmi {

class FMUBridge : public cppfmu::SlaveInstance, public simdeck::EntryPointPublisher, public simdeck::Object {
public:
    inline FMUBridge(Smp::ISimulator* sim, Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr)
        : Object(name, descr, parent),
          _sim(sim),
          _holdEP(addEP("Hold", "Hold the simulation", this, &FMUBridge::hold)) {}
    virtual ~FMUBridge() = default;

    static bool exportSim(Smp::ISimulator* sim, Smp::String8 filename);

    void SetupExperiment(cppfmu::FMIBoolean toleranceDefined, cppfmu::FMIReal tolerance, cppfmu::FMIReal tStart,
                         cppfmu::FMIBoolean stopTimeDefined, cppfmu::FMIReal tStop) override;
    inline void Terminate() override {
        _sim->Hold(true);
    }
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
    static std::vector<std::string> getBinariesFilenames();
    static wfrest::Json parseContainer(Smp::IContainer* cont);
    struct Field {
        const std::string name;
        const Smp::IField* field;
    };
    static void getAllFields(Smp::IContainer* cont, std::string parentName, std::vector<Field>& fields);
    static bool copyFile(std::string from, std::string to); /* TODO move the file copy function in the sys module? */
    static bool updateRPATH(std::string filepath);
    void hold() {
        _sim->Hold(true);
    }

    // std::unordered_map<const cppfmu::FMIValueReference, Smp::String8> _fmiRef2Str;
    Smp::ISimulator* _sim;
    Smp::IEntryPoint* _holdEP;
};

} /* namespace fmi */
} /* namespace simphonie */
#endif /* __simphonie_fmi_FMUBridge_HPP__ */
