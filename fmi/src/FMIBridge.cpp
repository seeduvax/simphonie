/*
 * @file FMIBridge.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/fmi/FMIBridge.hpp"

#include <filesystem>
#include <fstream>
#include <regex>
#include <vector>

#include "Smp/Publication/IType.h"
#include "Smp/Services/IEventManager.h"
#include "Smp/Services/ILogger.h"
#include "Smp/Services/IResolver.h"
#include "simdeck/ExInvalidFile.hpp"
#include "simphonie/fmi/FMILoggerBackend.hpp"
#include "simphonie/kern/Logger.hpp"
#include "simphonie/lua/LuaApi.hpp"
#include "simphonie/sys/Synchro.hpp"
#include "sol/sol.hpp"

namespace simphonie {
namespace fmi {
// --------------------------------------------------------------------
// ..........................................................
FMIBridge::FMIBridge(Smp::ISimulator* sim, Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Object(name, descr, parent), _sim(sim), _tk(_sim->GetTimeKeeper()), _sched(_sim->GetScheduler()) {
    _endAbsoluteSimTime = 0;
    _completed = false;
    {
        const auto ep = addEP("simTimeChangedEP", "", this, &FMIBridge::onSimTimeChanged);
        _sim->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_PostSimTimeChangeId, ep);
    }
    {
        const auto ep = addEP("leavExecutingEP", "", this, &FMIBridge::onLeaveExecuting);
        _sim->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_LeaveExecutingId, ep);
    }

    /* Setup _fmiRef2Field */
    for (auto cont : *(_sim->GetContainers())) {
        for (auto c : *(cont->GetComponents())) {
            getAllFields(c, _fmiRef2Field);
        }
    }
    std::sort(_fmiRef2Field.begin(), _fmiRef2Field.end(),
              [](const Smp::ISimpleField* a, const Smp::ISimpleField* b) { return compareFields(a, b); });
    std::ostringstream oss;
    oss << "Fields have been referenced as:" << std::endl << "<ValueReference>\t<short name>" << std::endl;
    for (size_t i = 0; i < _fmiRef2Field.size(); ++i) {
        oss << i << "\t" << _fmiRef2Field[i]->GetName() << std::endl;
    }
    _sim->GetLogger()->Log(this, oss.str().c_str(), Smp::Services::ILogger::LMK_Debug);
}
// ..........................................................
void FMIBridge::SetupExperiment(cppfmu::FMIBoolean toleranceDefined, cppfmu::FMIReal tolerance, cppfmu::FMIReal tStart,
                                cppfmu::FMIBoolean stopTimeDefined, cppfmu::FMIReal tStop) {
    /* TODO What to do with the experiment stop time? */
    if (toleranceDefined) {
        _sim->GetLogger()->Log(_sim, "tolerance argument is not used, even if provided",
                               Smp::Services::ILogger::LMK_Warning);
    }
    _tk->SetSimulationTime(static_cast<Smp::Duration>(tStart));
}
// ..........................................................
void FMIBridge::Terminate() {
    _sim->Hold(true);
}
// ..........................................................
void FMIBridge::Reset() {
    _sim->GetLogger()->Log(_sim, "The simulator cannot be resetted", Smp::Services::ILogger::LMK_Error);
}
// ..........................................................
void FMIBridge::SetReal(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIReal value[]) {
    SetGeneric(vr, nvr, value);
}
// ..........................................................
void FMIBridge::SetInteger(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIInteger value[]) {
    SetGeneric(vr, nvr, value);
}
// ..........................................................
void FMIBridge::SetBoolean(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIBoolean value[]) {
    SetGeneric(vr, nvr, value);
}
// ..........................................................
void FMIBridge::SetString(const cppfmu::FMIValueReference vr[], std::size_t nvr, const cppfmu::FMIString value[]) {
    SetGeneric(vr, nvr, value);
}
// ..........................................................
void FMIBridge::GetReal(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIReal value[]) const {
    GetGeneric(vr, nvr, value);
}
// ..........................................................
void FMIBridge::GetInteger(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIInteger value[]) const {
    GetGeneric(vr, nvr, value);
}
// ..........................................................
void FMIBridge::GetBoolean(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIBoolean value[]) const {
    GetGeneric(vr, nvr, value);
}
// ..........................................................
void FMIBridge::GetString(const cppfmu::FMIValueReference vr[], std::size_t nvr, cppfmu::FMIString value[]) const {
    GetGeneric(vr, nvr, value);
}
// ..........................................................
template <typename T>
void FMIBridge::SetGeneric(const cppfmu::FMIValueReference vr[], std::size_t nvr, T value[]) {
    for (std::size_t i = 0; i < nvr; ++i) {
        const auto id = vr[i];
        if (id >= _fmiRef2Field.size()) {
            std::ostringstream oss;
            oss << "Set requested on a too high ValueReference: " << id << ". Skipped.";
            _sim->GetLogger()->Log(this, oss.str().c_str(), Smp::Services::ILogger::LMK_Warning);
            continue;
        }
        auto fld = _fmiRef2Field[id];
        const Smp::AnySimple anysimp(fld->GetPrimitiveTypeKind(), value[i]);
        fld->SetValue(anysimp);
    }
}
// ..........................................................
template <typename T>
void FMIBridge::GetGeneric(const cppfmu::FMIValueReference vr[], std::size_t nvr, T value[]) const {
    for (std::size_t i = 0; i < nvr; ++i) {
        const auto id = vr[i];
        if (id >= _fmiRef2Field.size()) {
            std::ostringstream oss;
            oss << "Get requested on a too high ValueReference: " << id << ". Skipped.";
            _sim->GetLogger()->Log(this, oss.str().c_str(), Smp::Services::ILogger::LMK_Warning);
            continue;
        }
        auto fld = _fmiRef2Field.at(id);
        value[i] = static_cast<T>(fld->GetValue());
    }
}
// ..........................................................
bool FMIBridge::DoStep(cppfmu::FMIReal currentCommunicationPoint, cppfmu::FMIReal communicationStepSize,
                       cppfmu::FMIBoolean newStep, cppfmu::FMIReal& endOfStep) {
    /**
     * TOOD what is the use of newStep & endOfStep?
     * From cppfmu/fmi_functions.cpp: newStep=fmi2True and endOfStep=currentCommunicationPoint (ref to local) anyways
     */

    /* Set the end simulation time */
    _endAbsoluteSimTime = static_cast<Smp::Duration>((currentCommunicationPoint + communicationStepSize) * 1e9);

    /* Run the simulation */
    _sim->Run();

    /* Wait for its end */
    Synchronized(_mutex);
    while (!_completed) {
        MonitorWait(_monitor);
    }
    _completed = false;

    if (_tk->GetSimulationTime() < _endAbsoluteSimTime) {
        /* the simulation stopped before the requested simulation time */
        return false;
    }

    return true;
}
// ..........................................................
void FMIBridge::onLeaveExecuting() {
    {
        Synchronized(_mutex);
        _completed = true;
    }
    _monitor.notify_all();
}
// ..........................................................
void FMIBridge::onSimTimeChanged() {
    if (_tk->GetSimulationTime() >= _endAbsoluteSimTime) {
        _sim->Hold(true);
    }
}
// ..........................................................
void FMIBridge::getAllFields(Smp::IComponent* c, std::vector<Smp::ISimpleField*>& fields) {
    /* add fields */
    for (const auto f : *(c->GetFields())) {
        const auto sf = dynamic_cast<Smp::ISimpleField*>(f);
        if (sf != nullptr) {
            fields.push_back(sf);
        }
    }

    /* check for subcomponents */
    auto parent = dynamic_cast<Smp::IComposite*>(c);
    if (parent != nullptr) {
        for (auto cont : *(parent->GetContainers())) {
            for (auto newC : *(cont->GetComponents())) {
                FMIBridge::getAllFields(newC, fields);
            }
        }
    }
}
// ..........................................................
bool FMIBridge::compareFields(const Smp::ISimpleField* a, const Smp::ISimpleField* b) {
    /* build their hierarchies */
    std::vector<const Smp::IObject*> aHiera, bHiera;
    for (auto p = dynamic_cast<const Smp::IObject*>(a); p != nullptr; p = p->GetParent()) {
        aHiera.push_back(p);
    }
    for (auto p = dynamic_cast<const Smp::IObject*>(b); p != nullptr; p = p->GetParent()) {
        bHiera.push_back(p);
    }

    /* find the first different member */
    auto ia = aHiera.end() - 1;
    auto ib = bHiera.end() - 1;
    while (ia != aHiera.begin() && ib != bHiera.begin() && *ia == *ib) {
        ia--;
        ib--;
    }

    /* actual compare */
    const auto aBegin = (ia == aHiera.begin());
    const auto bBegin = (ib == bHiera.begin());
    if (aBegin && !bBegin) {
        /* the full name of a is shorter than b and equal to its beggining */
        return true;
    }
    if (!aBegin && bBegin) {
        /* the full name of b is shorter than a and equal to its beggining */
        return false;
    }
    return std::string((*ia)->GetName()).compare((*ib)->GetName()) < 0;
}

} /* namespace fmi */
} /* namespace simphonie */

cppfmu::UniquePtr<cppfmu::SlaveInstance> CppfmuInstantiateSlave(
    cppfmu::FMIString instanceName, cppfmu::FMIString fmuGUID, cppfmu::FMIString fmuResourceLocation,
    cppfmu::FMIString mimeType, cppfmu::FMIReal timeout, cppfmu::FMIBoolean visible, cppfmu::FMIBoolean interactive,
    cppfmu::Memory memory, cppfmu::Logger logger) {
    /* Set the path to the resources folder */
    /**
     * TODO We have to supprt the URI standard IETF RFC3986. However, sol::state::safe_script_file
     * looks like it is unable to read non-local files. A good workaround might be to download
     * a local copy of the file and to call sol's function on it.
     */
    if (std::strncmp(fmuResourceLocation, "file://", 7) != 0) {
        std::ostringstream oss;
        oss << "The URI to the fmu file should start with file://: \"" << fmuResourceLocation << "\" received";
        logger.Log(cppfmu::FMIStatus::fmi2Error, "Instantiation", oss.str().c_str());
        throw new cppfmu::FatalError(oss.str().c_str());
    }
    const auto resources = std::string(fmuResourceLocation).substr(7);

    /* Setup lua */
    {
        /* adding resources/../binaries/?/lib/ to sol2 libray path as well as adding resources/ to sol2 lua file path.
         * The latter is only useful if setup.lua has a 'require something.lua' */
        std::string path;
        for (const auto& dir : std::filesystem::directory_iterator(resources + "/../binaries/")) {
            path += (!path.empty() ? ";" : "") + dir.path().string() + "/lib/lib?.so";
        }
        path += ";;"; /* appends default paths too
                         https://stackoverflow.com/questions/26446333/how-to-set-the-lua-path-and-lua-cpath-for-the-zerobrane-studio-in-linux
                       */
#if defined(_WIN64) || defined(_WIN32)
        _putenv_s("LUA_CPATH", path.c_str());
        _putenv_s("LUA_PATH", (resources + "/?;;").c_str());
#else
        setenv("LUA_CPATH", path.c_str(), 1);
        setenv("LUA_PATH", (resources + "/?;;").c_str(), 1);
#endif /* defined(_WIN64) || defined(_WIN32) */
    }

    logger.Log(cppfmu::FMIStatus::fmi2Warning, "Instantiation",
               "Simphonie's logging system cannot be override for now: all the logs will be internally handle by "
               "simphonie during its setup.");

    /* Run the setup lua script to setup and retrieve the simulator */
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::os, sol::lib::math,
                       sol::lib::table, sol::lib::debug);
    const auto res = lua.safe_script_file((resources + "/setup.lua").c_str());
    if (!res.valid()) {
        const auto msg("The result from the Lua setup script failed");
        logger.Log(cppfmu::FMIStatus::fmi2Error, "Instantiation", msg);
        throw new cppfmu::FatalError(msg);
    }
    auto sim = res.get<Smp::ISimulator*>(0);

    /* Setup the logger */
    auto simLgr = dynamic_cast<simphonie::kern::Logger*>(sim->GetLogger());
    if (simLgr != nullptr) {
        auto fmiLgr = new simphonie::fmi::FMILoggerBackend(logger, "FMILogger",
                                                           "Sends logs to the host simulation logging system", simLgr);
        simLgr->clearBackends();
        simLgr->addBackend(fmiLgr);
    }
    else {
        logger.Log(
            cppfmu::FMIStatus::fmi2Warning, "Instantiation",
            "Simphonie's logging system cannot be override: all the logs will be internally handle by simphonie.");
    }

    auto fmu = cppfmu::AllocateUnique<simphonie::fmi::FMIBridge>(memory, sim, "FMIBridge");

    return fmu;
}
