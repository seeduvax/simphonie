/*
 * @file FMUBridge.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/fmi/FMUBridge.hpp"

#include <filesystem>
#include <fstream>
#include <regex>
#include <vector>

#include "Smp/Publication/IType.h"
#include "Smp/Services/IEventManager.h"
#include "Smp/Services/ILogger.h"
#include "Smp/Services/IResolver.h"
#include "simdeck/ExInvalidFile.hpp"
#include "simphonie/lua/LuaApi.hpp"
#include "simphonie/sys/Synchro.hpp"
#include "sol/sol.hpp"

#define LEAVEEXECUTINGEP "leave_executing"

namespace simphonie {
namespace fmi {
// --------------------------------------------------------------------
// ..........................................................
FMUBridge::FMUBridge(Smp::ISimulator* sim, Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Object(name, descr, parent),
      _sim(sim),
      _tk(_sim->GetTimeKeeper()),
      _sched(_sim->GetScheduler()),
      _holdEP(addEP("Hold", "Hold the simulation", this, &FMUBridge::hold)) {
    addEP(LEAVEEXECUTINGEP, "", this, &FMUBridge::onLeaveExecuting);
    _sim->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_LeaveExecutingId,
                                       GetEntryPoint(LEAVEEXECUTINGEP));
}
// ..........................................................
void FMUBridge::SetupExperiment(cppfmu::FMIBoolean toleranceDefined, cppfmu::FMIReal tolerance, cppfmu::FMIReal tStart,
                                cppfmu::FMIBoolean stopTimeDefined, cppfmu::FMIReal tStop) {
    if (toleranceDefined) {
        _sim->GetLogger()->Log(_sim, "tolerance argument is not used, even if provided",
                               Smp::Services::ILogger::LMK_Warning);
    }
    _sim->Publish();
    _tk->SetSimulationTime(static_cast<Smp::Duration>(tStart));
    if (stopTimeDefined) {
        /* TODO max priority */
        _sched->AddSimulationTimeEvent(_holdEP, static_cast<Smp::Duration>(tStop), 0, 0);
    }
    _sim->Configure();
    _sim->Connect();
}
// ..........................................................
void FMUBridge::Terminate() {
    _sim->Hold(true);
}
// ..........................................................
void FMUBridge::Reset() {
    _sim->GetLogger()->Log(_sim, "The simulator cannot be resetted", Smp::Services::ILogger::LMK_Error);
}
// ..........................................................
void FMUBridge::SetReal(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIReal value[]) {
    SetGeneric(vr, nvr, value);
}
// ..........................................................
void FMUBridge::SetInteger(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIInteger value[]) {
    SetGeneric(vr, nvr, value);
}
// ..........................................................
void FMUBridge::SetBoolean(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIBoolean value[]) {
    SetGeneric(vr, nvr, value);
}
// ..........................................................
void FMUBridge::SetString(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIString value[]) {
    SetGeneric(vr, nvr, value);
}
// ..........................................................
void FMUBridge::GetReal(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIReal value[]) const {
    GetGeneric(vr, nvr, value);
}
// ..........................................................
void FMUBridge::GetInteger(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIInteger value[]) const {
    GetGeneric(vr, nvr, value);
}
// ..........................................................
void FMUBridge::GetBoolean(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIBoolean value[]) const {
    GetGeneric(vr, nvr, value);
}
// ..........................................................
void FMUBridge::GetString(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIString value[]) const {
    GetGeneric(vr, nvr, value);
}
// ..........................................................
template <typename T>
void FMUBridge::SetGeneric(const cppfmu::FMIValueReference vr[], std::size_t nvr, T value[]) {
    for (std::size_t i = 0; i < nvr; ++i) {
        auto fld = _fmiRef2Field[vr[i]];
        /* TODO check if it needs conversion from cppfmu types to Smp ones on some platforms */
        if (fld.isArray) {
            const Smp::AnySimple anysimp(fld.value.array.ptr->GetType()->GetPrimitiveTypeKind(), value[i]);
            fld.value.array.ptr->SetValue(fld.value.array.index, anysimp);
        }
        else {
            const Smp::AnySimple anysimp(fld.value.simple->GetPrimitiveTypeKind(), value[i]);
            fld.value.simple->SetValue(anysimp);
        }
    }
}
// ..........................................................
template <typename T>
void FMUBridge::GetGeneric(const cppfmu::FMIValueReference vr[], std::size_t nvr, T value[]) const {
    for (std::size_t i = 0; i < nvr; ++i) {
        auto fld = _fmiRef2Field.at(vr[i]);
        if (fld.isArray) {
            value[i] = static_cast<T>(fld.value.array.ptr->GetValue(fld.value.array.index));
        }
        else {
            value[i] = static_cast<T>(fld.value.simple->GetValue());
        }
    }
}
// ..........................................................
bool FMUBridge::DoStep(cppfmu::FMIReal currentCommunicationPoint, cppfmu::FMIReal communicationStepSize,
                       cppfmu::FMIBoolean newStep, cppfmu::FMIReal& endOfStep) {
    /**
     * TOOD what is the use of newStep & endOfStep?
     * From cppfmu/fmi_functions.cpp: newStep=fmi2True and endOfStep=currentCommunicationPoint (ref to local) anyways
     */

    /* Set the holdEP */
    const auto endSimTime = static_cast<Smp::Duration>(communicationStepSize * 1e9);
    const auto event = _sched->AddSimulationTimeEvent(_holdEP, endSimTime, 0, 0); /* TODO max priority */
    /* TODO abboner sur sim change check to stop: this would avoid approximations errors */

    /* Run the simulation */
    _sim->Run();

    /* Wait for its end */
    Synchronized(_mutex);
    while (!_completed) {
        MonitorWait(_monitor);
    }
    _completed = false;

    if (_tk->GetSimulationTime() < endSimTime) {
        /* the simulation stopped before the requested simulation time */
        _sched->RemoveEvent(event); /* the event has not been use */
        return false;
    }

    return true;
}
// ..........................................................
bool FMUBridge::addFieldRef(cppfmu::FMIValueReference ref, const std::string& name) {
    FMUBridge::Field field;

    if (*name.end() != ']') {
        /* this is a simple field */
        field.isArray = false;
        field.value.simple = dynamic_cast<Smp::ISimpleField*>(_sim->GetResolver()->ResolveAbsolute(name.c_str()));
        if (field.value.simple == nullptr) {
            return false;
        }
        _fmiRef2Field.insert(std::make_pair(ref, field));
        return true;
    }

    /* this is an array element */
    field.isArray = true;
    const auto fieldName = name.substr(0, '[').c_str();
    field.value.array.ptr = dynamic_cast<Smp::ISimpleArrayField*>(_sim->GetResolver()->ResolveAbsolute(fieldName));
    if (field.value.array.ptr == nullptr) {
        return false;
    }
    field.value.array.index = std::stoi(name.substr('[', ']'));
    _fmiRef2Field.insert(std::make_pair(ref, field));
    return true;
}
// ..........................................................
void FMUBridge::hold() {
    _sim->Hold(true);
}
// ..........................................................
void FMUBridge::onLeaveExecuting() {
    {
        Synchronized(_mutex);
        _completed = true;
    }
    _monitor.notify_all();
}
// ..........................................................
std::vector<std::string> FMUBridge::getRegexMatches(const std::string& text, const char* pattern) {
    std::vector<std::string> res;
    const std::regex ptn(pattern);
    std::smatch match;
    auto begin = text.cbegin();
    const auto end = text.cend();
    while (std::regex_search(begin, end, match, ptn)) {
        res.push_back(match[0].str());
        begin = match.suffix().first;
    }
    return res;
}

} /* namespace fmi */
} /* namespace simphonie */

cppfmu::UniquePtr<cppfmu::SlaveInstance> CppfmuInstantiateSlave(
    cppfmu::FMIString instanceName, cppfmu::FMIString fmuGUID, cppfmu::FMIString fmuResourceLocation,
    cppfmu::FMIString mimeType, cppfmu::FMIReal timeout, cppfmu::FMIBoolean visible, cppfmu::FMIBoolean interactive,
    cppfmu::Memory memory, cppfmu::Logger logger) {
    /* TODO use the provided logger? */
    logger.Log(
        cppfmu::FMIStatus::fmi2Warning, "Instantiation",
        "Logging messages will not be sent to this logger. The Simphonie's internal logging system will handle them.");

    /* set the path to the resources folder */
    /**
     * TODO We have to supprt the URI standard IETF RFC3986. However, sol::state::safe_script_file
     * looks like unable to read non-local files. A good workaround might be to download
     * a local copy of the file and to call sol's fucntion on it.
     */
    if (std::strncmp(fmuResourceLocation, "file://", 7) != 0) {
        std::ostringstream oss;
        oss << "The URI to the fmu file should start with file://: \"" << fmuResourceLocation << "\" received";
        logger.Log(cppfmu::FMIStatus::fmi2Error, "Instantiation", oss.str().c_str());
        throw new cppfmu::FatalError(oss.str().c_str());
    }
    const auto resources = std::string(fmuResourceLocation).substr(7);

    /* run the setup lua script to setup and retrieve the simulator */
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::os, sol::lib::math,
                       sol::lib::table, sol::lib::debug);
    {
        /* adding ../binaries/?/lib/ to lua libray path */
        auto path = lua["package"]["cpath"].get<std::string>();
        for (const auto& dir : std::filesystem::directory_iterator(resources + "/../binaries/")) {
            path += (!path.empty() ? ";" : "") + dir.path().string() + "/lib/lib?.so";
        }
        lua["package"]["cpath"] = path;
    }
    const auto res = lua.safe_script_file((resources + "/setup.lua").c_str());
    if (!res.valid()) {
        const auto msg("The result from the Lua setup script failed");
        logger.Log(cppfmu::FMIStatus::fmi2Error, "Instantiation", msg);
        throw new cppfmu::FatalError(msg);
    }
    auto sim = res.get<Smp::ISimulator*>(0);

    auto fmu = cppfmu::AllocateUnique<simphonie::fmi::FMUBridge>(memory, sim, "FMUBridge");

    /* setup FMIValueReference to field mapping */
    /**
     * TODO reading the XML and looking for regex patterns is a dirty way to get
     * the mapping FMIValueReference to field pointers. A better way would have
     * been to use an hash function during the creation of the XML file for
     * hashing fields into an unique FMIValueReference.
     */
    {
        std::ifstream file((resources + "/../modelDescription.xml").c_str());
        std::ostringstream oss;
        oss << file.rdbuf();
        const auto modelDesc = oss.str();
        /* WARN the regex is too restrictive (should be removed anyway c.f. previous TODO) */
        const auto refsMatches = simphonie::fmi::FMUBridge::getRegexMatches(modelDesc, "valueReference=\"[0-9]+");
        const auto namesMatches =
            simphonie::fmi::FMUBridge::getRegexMatches(modelDesc, " name=\"[0-9a-zA-Z./]+(?:\[[0-9]+\])?");
        if (refsMatches.size() != namesMatches.size()) {
            const auto msg("The parsing of the fields' references failed.");
            logger.Log(cppfmu::FMIStatus::fmi2Error, "Instantiation", msg);
            throw new cppfmu::FatalError(msg);
        }
        for (size_t i = 0; i < namesMatches.size(); ++i) {
            const auto ref = static_cast<cppfmu::FMIValueReference>(std::stoi(refsMatches[i].substr(16)));
            const auto name = namesMatches[i].substr(7).c_str();
            if (!fmu.get()->addFieldRef(ref, name)) {
                std::ostringstream oss;
                oss << "An error occur while looking for the field associated to the name \"" << name << "\"";
                logger.Log(cppfmu::FMIStatus::fmi2Error, "Instantiation", oss.str().c_str());
                throw new cppfmu::FatalError(oss.str().c_str());
            }
        }
    }

    return fmu;
}
