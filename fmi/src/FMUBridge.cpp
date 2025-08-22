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

#include <cstring>
#include <filesystem>
#include <fstream>

#include "Smp/Services/IEventManager.h"
#include "Smp/Services/ILogger.h"
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
void FMUBridge::SetReal(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIReal value[]) {}
// ..........................................................
void FMUBridge::SetInteger(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIInteger value[]) {}
// ..........................................................
void FMUBridge::SetBoolean(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIBoolean value[]) {}
// ..........................................................
void FMUBridge::SetString(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIString value[]) {}
// ..........................................................
void FMUBridge::GetReal(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIReal value[]) const {}
// ..........................................................
void FMUBridge::GetInteger(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIInteger value[]) const {}
// ..........................................................
void FMUBridge::GetBoolean(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIBoolean value[]) const {}
// ..........................................................
void FMUBridge::GetString(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIString value[]) const {}
// ..........................................................
bool FMUBridge::DoStep(cppfmu::FMIReal currentCommunicationPoint, cppfmu::FMIReal communicationStepSize,
                       cppfmu::FMIBoolean newStep, cppfmu::FMIReal& endOfStep) {
    /**
     * TOOD what is the use of newStep & endOfStep?
     * From cppfmu/fmi_functions.cpp: newStep=fmi2True and endOfStep=currentCommunicationPoint (ref to local) anyways
     */

    /* Set the holdEP */
    const auto endSimTime = static_cast<Smp::Duration>((currentCommunicationPoint + communicationStepSize) * 1e9);
    std::cout << currentCommunicationPoint << " " << communicationStepSize << " " << endSimTime << std::endl;
    const auto event = _sched->AddSimulationTimeEvent(_holdEP, endSimTime, 0, 0); /* TODO max priority */

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

    /* set the path to the resources folder. The issue is likely that sol::state::safe_script_file only accepts "local"
     * files */
    if (std::strncmp(fmuResourceLocation, "file://", 7) != 0) {
        /* TODO support IETF RFC3986 */
        std::ostringstream oss;
        oss << "The URI to the fmu file should be starting with file://: \"" << fmuResourceLocation << "\" received";
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

    /* setup FMIValueReference to string names mapping */
    /* TODO */

    return fmu;
}
