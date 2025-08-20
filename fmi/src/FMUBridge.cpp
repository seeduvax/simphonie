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

#include <sys/stat.h>

#include <ctime>
#include <iomanip>
#include <memory>

#include "Smp/IModel.h"
#include "Smp/Publication/IType.h"
#include "Smp/Services/ILogger.h"
#include "Smp/Services/IScheduler.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simdeck/ExInvalidFile.hpp"
#include "simdeck/Utils.hpp"
#include "simphonie/kern/Simulator.hpp"
#ifdef __linux__
#include <dlfcn.h>
#include <link.h>
#else
#include <windows.h>
#endif

namespace simphonie {
namespace fmi {
// --------------------------------------------------------------------
// ..........................................................
std::vector<std::string> FMUBridge::getBinariesFilenames() {
#ifdef __linux__
    std::vector<std::string> names;

    /* find this lib for entry point */
    {
        Dl_info info;
        dladdr((void*)CppfmuInstantiateSlave, &info);
        if (!info.dli_fname || info.dli_fname[0] == '\0') {
            return {};
        }
        names.push_back(info.dli_fname);
    }

    /* find other shared libraries */
    auto callback = [](struct dl_phdr_info* info, size_t sz, void* data) {
        const auto name = info->dlpi_name;
        const auto names = (std::vector<std::string>*)data;
        if (name && name[0] != '\0' && name != (*names)[0]) {
            names->push_back(name);
        }
        return 0;
    };
    dl_iterate_phdr(callback, &names);

    return names;
#else
    _sim->GetLogger()->Log(_sim, "NOT IMPLEMENTED YET for Windows", Smp::Services::ILogger::LMK_Error) exit(0);
    /* https://stackoverflow.com/questions/6924195/get-dll-path-at-runtime */
#endif
}
// ..........................................................
wfrest::Json FMUBridge::parseContainer(Smp::IContainer* cont) {
    wfrest::Json::Object json;
    for (const auto comp : *(cont->GetComponents())) {
        wfrest::Json::Object compjson;
        compjson.push_back("type", simdeck::Utils::Demangle(typeid(*comp).name()));
        compjson.push_back("description", comp->GetDescription());
        auto composite = dynamic_cast<Smp::IComposite*>(comp);
        if (composite != nullptr) {
            for (auto subcont : *(composite->GetContainers())) {
                compjson.push_back(subcont->GetName(), parseContainer(subcont));
            }
        }
        json.push_back(comp->GetName(), compjson);
    }
    return json;
}
// ..........................................................
void FMUBridge::getAllFields(Smp::IContainer* cont, std::string parentName, std::vector<FMUBridge::Field>& fields) {
    for (const auto comp : *(cont->GetComponents())) {
        const std::string compName = parentName + "." + comp->GetName();
        for (auto fld : *(comp->GetFields())) {
            const std::string name = compName + "." + fld->GetName();
            fields.push_back({name, fld});
        }
        auto composite = dynamic_cast<Smp::IComposite*>(comp);
        if (composite != nullptr) {
            for (auto subcont : *(composite->GetContainers())) {
                getAllFields(subcont, compName, fields);
            }
        }
    }
}
// ..........................................................
bool FMUBridge::copyFile(std::string from, std::string to) {
    std::ifstream src(from, std::ios::binary);
    std::ofstream dst(to, std::ios::binary);
    if (!src.is_open() || !dst.is_open()) {
        return false;
    }
    dst << src.rdbuf();
    dst.close();
    return true;
}
// ..........................................................
bool FMUBridge::updateRPATH(std::string filepath) {
    std::vector<char> buf;
    /* rd buffer */
    {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        buf = std::vector<char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
    }
    /* search the old string */
    const std::string oldstr("$ORIGIN/../lib");
    auto it = std::search(buf.begin(), buf.end(), oldstr.begin(), oldstr.end());
    if (it == buf.end()) {
        return false;
    }
    /* set the new string */
    std::memcpy(&(*it), "$ORIGIN/lib\0\0\0", 14);
    /* save */
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file.write(buf.data(), buf.size());
    file.close();
    return true;
}
// ..........................................................
bool FMUBridge::exportSim(Smp::ISimulator* sim, Smp::String8 filename) {
    const std::string modelIdentifier = "Simphonie";
    std::string base(filename);
    base += ".d";

    /* create the root folder */
    if (mkdir(base.c_str(), ALLPERMS) != 0) {
        sim->GetLogger()->Log(sim, "Failed to export to FMU", Smp::Services::ILogger::LMK_Error);
        return false;
    }

    /* create the binaries subfolder */
    std::string path(base);
    path += "/binaries";
    if (mkdir(path.c_str(), ACCESSPERMS) != 0) {
        sim->GetLogger()->Log(sim, "Failed to export to FMU", Smp::Services::ILogger::LMK_Error);
        return false;
    }

    /* create the architecture subfolder */
    path += "/";
#ifdef _WIN64
    path += "win64";
#elif defined(_WIN32)
    path += "win32";
#elif defined(__linux__)
#ifdef __x86_64__
    path += "linux64";
#else
    path += "linux32";
#endif
#else
    sim->GetLogger()->Log(sim, "Failed to export to FMU", Smp::Services::ILogger::LMK_Error);
    return false;
#endif
    if (mkdir(path.c_str(), ACCESSPERMS) != 0) {
        sim->GetLogger()->Log(sim, "Failed to export to FMU", Smp::Services::ILogger::LMK_Error);
        return false;
    }

    /* copy the binaries into the fmu */
    {
        const auto binariesFilenames = getBinariesFilenames();
        if (binariesFilenames.size() <= 0) {
            return false;
        }
        path += "/";
        /* main entry point */
        sim->GetLogger()->Log(sim, ("Entry point binary found at " + binariesFilenames[0]).c_str(),
                              Smp::Services::ILogger::LMK_Debug);
        auto mainpath = path;
        mainpath += modelIdentifier;
#ifdef __linux__
        mainpath += ".so";
#else
        mainpath += ".dll";
#endif
        if (!copyFile(binariesFilenames[0], mainpath)) {
            std::ostringstream oss;
            oss << "Failed to copy the file from \"" << binariesFilenames[0] << "\" to \"" << mainpath << "\"";
            sim->GetLogger()->Log(sim, oss.str().c_str(), Smp::Services::ILogger::LMK_Error);
            return false;
        }
        if (!updateRPATH(mainpath)) {
            std::ostringstream oss;
            oss << "Failed to change rpath for \"" << mainpath
                << "\". This may entailed missing dependencies issues within the FMU";
            sim->GetLogger()->Log(sim, oss.str().c_str(), Smp::Services::ILogger::LMK_Warning);
        }
        /* libraries */
        path += "lib/";
        if (mkdir(path.c_str(), ACCESSPERMS) != 0) {
            sim->GetLogger()->Log(sim, "Failed to export to FMU", Smp::Services::ILogger::LMK_Error);
            return false;
        }
        for (size_t i = 1; i < binariesFilenames.size(); ++i) {
            const auto from = binariesFilenames[i];
            const auto name = from.substr(from.find_last_of("/") + 1);
            const auto to = path + name;
            if (!copyFile(from, to)) {
                std::ostringstream oss;
                oss << "Failed to copy the file from \"" << from << "\" to \"" << to
                    << "\". This may entailed missing dependencies issues within the FMU";
                sim->GetLogger()->Log(sim, oss.str().c_str(), Smp::Services::ILogger::LMK_Warning);
            }
        }
    }

    /* create the resources */
    path = base;
    path += "/resources";
    if (mkdir(path.c_str(), ACCESSPERMS) != 0) {
        sim->GetLogger()->Log(sim, "Failed to export to FMU", Smp::Services::ILogger::LMK_Error);
        return false;
    }
    {
        const auto p = path + "/initial-state.cp";
        sim->Store(p.c_str());
    }
    {
        const auto p = path + "/architecture.json";
        wfrest::Json json;
        for (const auto cont : *(sim->GetContainers())) {
            json.push_back(cont->GetName(), parseContainer(cont));
        }
        std::ofstream file(p);
        file << json.dump();
    }

    /* create the modelDescription.xml file */
    auto t = std::time(nullptr);
    auto tm = std::localtime(&t);
    path = base;
    path += "/modelDescription.xml";
    std::ofstream xml(path.c_str());
    if (!xml.is_open()) {
        sim->GetLogger()->Log(sim, "Failed to export to FMU", Smp::Services::ILogger::LMK_Error);
        return false;
    }
    xml << R"(<?xml version="1.0" encoding="UTF-8"?>)" << std::endl
        << R"(<fmiModelDescription)" << std::endl
        << R"(  fmiVersion="2.0")" << std::endl /* 2.0.5 actually */
        << R"(  modelName="Simphonie")" << std::endl
        << R"(  guid="SIMPHONIE")" << std::endl /* TODO do we care? */
        << R"(  description="Simphonie-based co-simulation")" << std::endl
        /* << R"(  version="@OSMPVERSION@")" TODO could be great */
        << R"(  generationTool="Simphonie")" << std::endl
        << R"(  generationDateAndTime=")"
        << std::put_time(tm, "%Y-%m-%dT%H:%M:%SZ")
        << R"(")" << std::endl
        << R"(  variableNamingConvention="structured">)" << std::endl
        << R"(  <CoSimulation)" << std::endl
        << R"(    modelIdentifier=")"
        << modelIdentifier
        << R"(")" << std::endl
        << R"(    needsExecutionTool="false")" << std::endl
        << R"(    canHandleVariableCommunicationStepSize="true")" << std::endl
        << R"(    canInterpolateInputs="false")" << std::endl
        << R"(    maxOutputDerivativeOrder="0")" << std::endl
        << R"(    canRunAsynchronuously="false")" << std::endl
        << R"(    canBeInstantiatedOnlyOncePerProcess="false")" << std::endl
        << R"(    canNotUseMemoryManagementFunctions="false")" << std::endl
        << R"(    canGetAndSetFMUstate="false")" << std::endl
        << R"(    canSerializeFMUstate="false")" << std::endl
        << R"(    providesDirectionalDerivative="false"/>)" << std::endl
        << std::endl
        << R"(  <DefaultExperiment startTime="0" stepSize="1"/>)" << std::endl
        << std::endl
        << R"(  <ModelVariables>)" << std::endl;
    std::vector<FMUBridge::Field> fields;
    const std::string basename(sim->GetName());
    for (auto cont : *(sim->GetContainers())) {
        getAllFields(cont, basename, fields);
    }
    int ref = 0;
    std::ostringstream modelStruct;
    for (auto fld : fields) {
        std::string causality;
        if (fld.field->IsInput()) {
            causality = "input";
        }
        else {
            if (fld.field->IsOutput()) {
                causality = "output";
                modelStruct << R"(      <Unknown index=")";
                modelStruct << ref;
                modelStruct << R"("/>)" << std::endl;
            }
            else {
                causality = "local";
            }
        }
        std::string type;
        std::string variability("discrete");
        const auto uuid = fld.field->GetType()->GetUuid();
        if (uuid == Smp::Uuids::Uuid_Bool || uuid == Smp::Uuids::Uuid_Char8
            || uuid == Smp::Uuids::Uuid_ComponentStateKind || uuid == Smp::Uuids::Uuid_DateTime
            || uuid == Smp::Uuids::Uuid_Duration || uuid == Smp::Uuids::Uuid_EventId || uuid == Smp::Uuids::Uuid_Int16
            || uuid == Smp::Uuids::Uuid_Int32 || uuid == Smp::Uuids::Uuid_Int64 || uuid == Smp::Uuids::Uuid_Int8
            || uuid == Smp::Uuids::Uuid_LogMessageKind || uuid == Smp::Uuids::Uuid_ParameterDirectionKind
            || uuid == Smp::Uuids::Uuid_PrimitiveTypeKind || uuid == Smp::Uuids::Uuid_SimulatorStateKind
            || uuid == Smp::Uuids::Uuid_TimeKind || uuid == Smp::Uuids::Uuid_UInt16 || uuid == Smp::Uuids::Uuid_UInt32
            || uuid == Smp::Uuids::Uuid_UInt64 || uuid == Smp::Uuids::Uuid_UInt8 || uuid == Smp::Uuids::Uuid_Uuid
            || uuid == Smp::Uuids::Uuid_ViewKind) {
            type = "Integer";
        }
        else {
            if (uuid == Smp::Uuids::Uuid_Float32 || uuid == Smp::Uuids::Uuid_Float64) {
                type = "Real";
                variability = "continuous";
            }
            else {
                type = "String"; /* default */
            }
        }

        xml << R"(    <ScalarVariable)" << std::endl
            << R"(      name=")"
            << fld.name
            << R"(")" << std::endl
            << R"(      valueReference=")"
            << ref
            << R"(")" << std::endl
            << R"(      description=")"
            << fld.field->GetDescription()
            << R"(")" << std::endl
            << R"(      causality=")"
            << causality
            << R"(")" << std::endl
            << R"(      initial="exact")" << std::endl
            << R"(      variability=")"
            << variability
            << R"(">)" << std::endl
            << R"(      <)"
            << type
            << R"( start=")"
            << "0" /* TODO Store mechanism */
            << R"("/>)" << std::endl
            << R"(    </ScalarVariable>)" << std::endl;
        ref++;
    }
    xml << "  </ModelVariables>" << std::endl
        << "  <ModelStructure>" << std::endl
        << "    <Outputs>" << std::endl
        << modelStruct.str() << "    </Outputs>" << std::endl
        << "  </ModelStructure>" << std::endl
        << "</fmiModelDescription>" << std::endl;
    xml.close();

    /* zip the file */
    std::string cmd;
    cmd += "cd ";
    cmd += base;
    cmd += " && zip -r --quiet ../";
    cmd += filename;
    cmd += " * && cd .. && rm -r ";
    cmd += base;
    if (std::system(cmd.c_str()) != 0) { /* TODO really dirty (lizip could be great)*/
        sim->GetLogger()->Log(sim, "ZIP the created folder to get the exported fmu",
                              Smp::Services::ILogger::LMK_Warning);
        return false;
    }

    sim->GetLogger()->Log(sim, "Simulator successfully exported to FMU!", Smp::Services::ILogger::LMK_Information);
    return true;
}
// ..........................................................
void FMUBridge::SetupExperiment(cppfmu::FMIBoolean toleranceDefined, cppfmu::FMIReal tolerance, cppfmu::FMIReal tStart,
                                cppfmu::FMIBoolean stopTimeDefined, cppfmu::FMIReal tStop) {
    if (toleranceDefined) {
        _sim->GetLogger()->Log(_sim, "tolerance argument is not used, even if provided",
                               Smp::Services::ILogger::LMK_Warning);
    }
    _sim->Publish();
    _sim->GetTimeKeeper()->SetSimulationTime(static_cast<Smp::Duration>(tStart));
    if (stopTimeDefined) {
        /* TODO max priority */
        _sim->GetScheduler()->AddSimulationTimeEvent(_holdEP, static_cast<Smp::Duration>(tStop), 0, 0);
    }
    _sim->Configure();
    _sim->Connect();
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
    return true;
}
} /* namespace fmi */
} /* namespace simphonie */

Smp::IComponent* _createComponent(wfrest::Json elem, Smp::IComposite* parent, Smp::ISimulator* sim) {
    std::string name, type, desc;
    if (!elem.has("type")) {
        throw simdeck::ExInvalidFile(sim, "architecture.json",
                                     "The architecture.json file is malformed as an element has no type provided");
    }
    name = elem.key();
    type = elem["type"].get<std::string>();
    if (elem.has("description")) {
        desc = elem["description"].get<std::string>();
    }
    for (auto fac : *(sim->GetFactories())) {
        if (type == fac->GetTypeName()) {
            return fac->CreateInstance(name.c_str(), desc.c_str(), parent);
        }
    }
    return nullptr;
}
// ..........................................................
void _addSubComponents(Smp::IComposite* parent, wfrest::Json elem, Smp::ISimulator* sim) {
    for (auto cont : elem) {
        if (cont.is_object()) { /* skip description and type entries */
            auto container = parent->GetContainer(elem.key().c_str());
            if (container == nullptr)
                continue;
            for (auto child : cont) {
                auto sub = _createComponent(child, parent, sim);
                if (sub != nullptr) {
                    container->AddComponent(sub);
                }
                auto composite = dynamic_cast<Smp::IComposite*>(sub);
                if (composite != nullptr) {
                    _addSubComponents(composite, child, sim);
                }
            }
        }
    }
}
// ..........................................................
cppfmu::UniquePtr<cppfmu::SlaveInstance> CppfmuInstantiateSlave(
    cppfmu::FMIString instanceName, cppfmu::FMIString fmuGUID, cppfmu::FMIString fmuResourceLocation,
    cppfmu::FMIString mimeType, cppfmu::FMIReal timeout, cppfmu::FMIBoolean visible, cppfmu::FMIBoolean interactive,
    cppfmu::Memory memory, cppfmu::Logger logger) {
    /* TODO use the provided logger? */
    logger.Log(
        cppfmu::FMIStatus::fmi2Warning, "",
        "Logging messages will not be sent to this logger. The Simphonie's internal logging system will handle them.");

    /* instantiate the simulator */
    auto sim = new simphonie::kern::Simulator(instanceName, "", nullptr);
    auto fmu = cppfmu::AllocateUnique<simphonie::fmi::FMUBridge>(memory, sim, "FMUBridge");
    const std::string base(fmuResourceLocation);

    /* setup the simulator according to the architecture.json file */
    wfrest::Json arch;
    {
        std::ostringstream oss;
        std::ofstream file(base + "architecture.json");
        oss << file.rdbuf();
        arch.parse(oss.str());
    }
    for (auto elem : arch) {
        auto comp = _createComponent(elem, sim, sim);
        if (comp != nullptr) {
            auto service = dynamic_cast<Smp::IService*>(comp);
            auto model = dynamic_cast<Smp::IModel*>(comp);
            if (service != nullptr) {
                sim->AddService(service);
            }
            if (model != nullptr) {
                sim->AddModel(model);
            }
            if (model == nullptr && service == nullptr) {
                sim->GetLogger()->Log(comp,
                                      "Component is neither a service nor a model. Cannot be added to the simulator",
                                      Smp::Services::ILogger::LMK_Error);
                delete comp;
                comp = nullptr;
            }
        }
        auto composite = dynamic_cast<Smp::IComposite*>(comp);
        if (composite != nullptr) {
            _addSubComponents(composite, elem, sim);
        }
        break;
    }

    /* restore the initial state values from the initial-state.cp file */
    const auto initState = base + "initial-state.cp";
    sim->Restore(initState.c_str());

    /* setup FMIValueReference to string names mapping */
    /* TODO */

    return fmu;
}
