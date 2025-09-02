/*
 * @file FMILoad.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/fmi/FMILoad.hpp"

#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/xpath.h>

#include <filesystem>
#include <sstream>

#include "Smp/IPublication.h"
#include "Smp/ISimulator.h"
#include "simdeck/ExInvalidFile.hpp"
#include "simdeck/Exception.hpp"
#include "simdeck/StringType.hpp"

#define TMP_FOLDER "./FMILoad.d"

#define toStr(a) #a
#define getFn(name)                                                                            \
    auto name = _binary->getEntry<fmi2##name##TYPE*>(toStr(fmi2##name));                       \
    if (name == nullptr) {                                                                     \
        const auto msg(toStr(The entry point fmi2##name cannot be found in the provided FMU)); \
        _logger->Log(this, msg, Smp::Services::ILogger::LMK_Error);                            \
        throw new simdeck::ExInvalidFile(this, _path.c_str(), msg);                            \
    }

namespace simphonie {
namespace fmi {
// --------------------------------------------------------------------
// ..........................................................
FMILoad::FMILoad(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simdeck::Component(name, descr, parent),
      _instanceName(name),
      _tolerance(-1.0),
      _startTime(-1.0),
      _stopTime(-1.0),
      _stepSize(-1.0) {
    addEP("doStep", "Do a single step of simulation", this, &FMILoad::doStepEP);
}
// ..........................................................
FMILoad::~FMILoad() {
    if (_fmiObj != nullptr) {
        auto FreeInstance = _binary->getEntry<fmi2FreeInstanceTYPE*>("fmi2FreeInstance");
        if (FreeInstance == nullptr) {
            const auto msg("The entry point #name cannot be found in the provided FMU");
            _logger->Log(this, msg, Smp::Services::ILogger::LMK_Error);
        }
        FreeInstance(_fmiObj);
    }
    if (std::filesystem::is_directory(TMP_FOLDER)) {
        std::ostringstream cmd;
        cmd << "rm -r " << TMP_FOLDER;
        std::system(cmd.str().c_str()); /* dirty */
    }
}
// ..........................................................
void FMILoad::publish(Smp::IPublication* receiver) {
    receiver->PublishField("path", "Path to the fmu file (zipped or not)", &_path, simdeck::StringType::UuidString,
                           Smp::ViewKind::VK_All, false, true, false);
    receiver->PublishField("instanceName", "Name of the instance (c.f. fmi2Instantiate's documentation)",
                           &_instanceName, simdeck::StringType::UuidString, Smp::ViewKind::VK_All, false, true, false);
    receiver->PublishField("tolerance",
                           "Tolerance of the error estimation of the communication interval (c.f. "
                           "fmi2SetupExperiment's documentation): -1.0 for the default value",
                           &_tolerance, Smp::ViewKind::VK_All, false, true, false);
    receiver->PublishField(
        "startTime",
        "Staring time of the simulation (c.f. fmi2SetupExperiment's documentation): -1.0 for the default value",
        &_startTime, Smp::ViewKind::VK_All, false, true, false);
    receiver->PublishField(
        "stopTime",
        "Stopping time of the simulation (c.f. fmi2SetupExperiment's documentation): -1.0 for the default value",
        &_stopTime, Smp::ViewKind::VK_All, false, true, false);
    receiver->PublishField("stepSize",
                           "Step size of doStep function (c.f. fmi2DoStep's documentation): -1.0 for the default value",
                           &_stepSize, Smp::ViewKind::VK_All, false, true, false);
}
// ..........................................................
void FMILoad::configure() {
    _logger = getSimulator()->GetLogger();
    _tk = getSimulator()->GetTimeKeeper();
    _cbArgs.logger = _logger;
    _cbArgs.self = this;

    /* Unzip the file if needed */
    if (!std::filesystem::exists(_path)) {
        std::ostringstream msg;
        msg << "The path '" << _path << "' neither lead to a file nor a directory";
        _logger->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Error);
        throw new simdeck::ExInvalidFile(this, _path.c_str(), msg.str().c_str());
    }
    if (!std::filesystem::is_directory(_path)) {
        /* TODO path sanitazition to avoid dangerous behaviours */
        std::ostringstream cmd;
        cmd << "mkdir " << TMP_FOLDER << " && unzip -q \"" << _path << "\" -d " << TMP_FOLDER;
        const auto res = std::system(cmd.str().c_str()); /* dirty */
        if (res != 0) {
            std::ostringstream msg;
            msg << "The file '" << _path << "' cannot be unzipped in " << TMP_FOLDER;
            _logger->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Error);
            throw new simdeck::ExInvalidFile(this, _path.c_str(), msg.str().c_str());
        }
        _path = TMP_FOLDER;
    }

    /* Analyse the XML */
    auto doc = xmlReadFile((_path + "/modelDescription.xml").c_str(), NULL, 0);
    if (doc == nullptr) {
        const auto msg("The modelDescription.xml file of the FMU cannot be parsed");
        _logger->Log(this, msg, Smp::Services::ILogger::LMK_Error);
        throw new simdeck::ExInvalidFile(this, _path.c_str(), msg);
    }
    auto root = xmlDocGetRootElement(doc);
    if (root == nullptr) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        const auto msg("The modelDescription.xml file of the FMU cannot be parsed: the root node is not found");
        _logger->Log(this, msg, Smp::Services::ILogger::LMK_Error);
        throw new simdeck::ExInvalidFile(this, _path.c_str(), msg);
    }

    /* get the name of the simulator */
    auto simName = reinterpret_cast<char*>(xmlGetProp(root, reinterpret_cast<const xmlChar*>("modelName")));
    if (simName == nullptr) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        const auto msg(
            "The modelDescription.xml file of the FMU cannot be parsed: the modelName attribute is not found");
        _logger->Log(this, msg, Smp::Services::ILogger::LMK_Error);
        throw new simdeck::ExInvalidFile(this, _path.c_str(), msg);
    }
    /* get its guid */
    auto guid = reinterpret_cast<char*>(xmlGetProp(root, reinterpret_cast<const xmlChar*>("guid")));
    if (guid == nullptr) {
        xmlFree(simName);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        const auto msg("The modelDescription.xml file of the FMU cannot be parsed: the guid attribute is not found");
        _logger->Log(this, msg, Smp::Services::ILogger::LMK_Error);
        throw new simdeck::ExInvalidFile(this, _path.c_str(), msg);
    }
    /* get the default setupExperiment's settings */
    {
        auto ctx = xmlXPathNewContext(doc);
        if (ctx == nullptr) {
            _logger->Log(this, "No default experiment for the FMI model: xml context failed",
                         Smp::Services::ILogger::LMK_Debug);
        }
        else {
            auto defaultExp =
                xmlXPathEvalExpression(reinterpret_cast<const xmlChar*>("/fmiModelDescription/DefaultExperiment"), ctx);
            if (defaultExp == nullptr) {
                _logger->Log(this, "No default experiment for the FMI model", Smp::Services::ILogger::LMK_Debug);
            }
            else {
                const auto node = defaultExp->nodesetval->nodeTab[0];
                if (_startTime < 0.0) {
                    auto res =
                        reinterpret_cast<const char*>(xmlGetProp(node, reinterpret_cast<const xmlChar*>("startTime")));
                    if (res != nullptr) {
                        _startTime = std::stod(res);
                    }
                }
                if (_stopTime < 0.0) {
                    auto res =
                        reinterpret_cast<const char*>(xmlGetProp(node, reinterpret_cast<const xmlChar*>("stopTime")));
                    if (res != nullptr) {
                        _stopTime = std::stod(res);
                    }
                }
                if (_tolerance < 0.0) {
                    auto res =
                        reinterpret_cast<const char*>(xmlGetProp(node, reinterpret_cast<const xmlChar*>("tolerance")));
                    if (res != nullptr) {
                        _tolerance = std::stod(res);
                    }
                }
                if (_stepSize < 0.0) {
                    auto res =
                        reinterpret_cast<const char*>(xmlGetProp(node, reinterpret_cast<const xmlChar*>("stepSize")));
                    if (res != nullptr) {
                        _stepSize = std::stod(res);
                    }
                }
            }
        }
    }
    /* publish fields */
    /* TODO */
    /* free xml's stuff */
    xmlFreeDoc(doc);
    xmlCleanupParser();

    auto libpath = _path + "/binaries";
#ifdef _WIN64
    libpath += "/win64";
#elif defined(_WIN32)
    libpath += "/win32";
#elif defined(__linux__)
#ifdef __x86_64__
    libpath += "/linux64";
#else
    libpath += "/linux32";
#endif
#else
    static_assert(false, "unknown platform");
#endif
    libpath += "/";
    libpath += simName;
    libpath += ".so";
    xmlFree(simName);
    /* cosimulation */
    /* publish the fields */

    /* Check if platform compatible */
    if (!std::filesystem::exists(libpath)) {
        std::ostringstream msg;
        msg << "The FMI '" << _path << "' is not compatible with this platform";
        _logger->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Error);
        throw new simdeck::ExInvalidFile(this, _path.c_str(), msg.str().c_str());
    }

    /* Instantiate the FMI object */
    _binary = std::make_unique<simphonie::sys::DLib>(libpath.c_str());
    getFn(Instantiate) const auto resourcesURI = (_path + "/resources").c_str(); /* TODO URI */
    _fmiObj =
        Instantiate(_instanceName.c_str(), fmi2CoSimulation, guid, resourcesURI, &_cbFunctions, fmi2False, fmi2True);
    xmlFree(guid);

    /* Setup */
    setup();

    /* Retrieve DoStep function */
    getFn(DoStep) _doStepFn = DoStep;

    if (_startTime < 0.0)
        _startTime = 0.0; /* no value provided by the user as well as no value found in the xml */
    if (_stepSize < 0.0)
        _stepSize = 1.0; /* no value provided by the user as well as no value found in the xml */
    _curComTime = _startTime;
    {
        std::ostringstream msg;
        msg << "FMI model successfully setup with startTime=" << _startTime << "s, stepSize=" << _stepSize << "s";
        if (_stopTime >= 0.0)
            msg << ", stopTime=" << _stopTime << "s";
        if (_tolerance >= 0.0)
            msg << ", tolerance=" << _tolerance;
        _logger->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Information);
    }
}
// ..........................................................
void FMILoad::setup() {
    /**
     * 1:
     * - set variables with variablility!=constant and (initial=exact or initial=approx)
     * - call fmi2SetupExperiment
     */
    getFn(SetupExperiment)
        SetupExperiment(_fmiObj, (_tolerance >= 0), _tolerance, _startTime, (_stopTime >= 0), _stopTime);
    /* 2: set variables with variablility!=constant and (initial=exact or causality=input) */
    /* 3: call fmi2EnterInitializationMode */
    getFn(EnterInitializationMode) EnterInitializationMode(_fmiObj);
    /**
     * 4:
     * - set variables with causability=input or (causability=parameter and variability=tunable)
     * - set variables with variablility!=constant and initial=exact
     */
    /* 5: call fmi2ExitInitializationMode */
    getFn(ExitInitializationMode) ExitInitializationMode(_fmiObj);
}
// ..........................................................
void FMILoad::loggingFn(fmi2ComponentEnvironment componentEnvironment, fmi2String instanceName, fmi2Status status,
                        fmi2String category, fmi2String message, ...) {
    auto args = static_cast<cbArgs*>(componentEnvironment);
    std::ostringstream msg;
    msg << "[" << category << "] " << message;
    switch (status) {
        case fmi2Discard:
        case fmi2OK:
        case fmi2Pending:
            args->logger->Log(args->self, msg.str().c_str(), Smp::Services::ILogger::LMK_Information);
            break;
        case fmi2Warning:
            args->logger->Log(args->self, msg.str().c_str(), Smp::Services::ILogger::LMK_Warning);
            break;
        case fmi2Error:
        case fmi2Fatal:
            args->logger->Log(args->self, msg.str().c_str(), Smp::Services::ILogger::LMK_Error);
            break;
    }
}
// ..........................................................
bool FMILoad::doStep(fmi2Real stepSize) {
    if (_curComTime + stepSize >= _stopTime) {
        _logger->Log(this, "Cannot call doStep function as the FMI co-simulation time reached stopTime",
                     Smp::Services::ILogger::LMK_Warning);
        return false;
    }
    /* Get/set variables */
    /* Call fmi2DoStep */
    const auto status = _doStepFn(_fmiObj, _curComTime, stepSize, fmi2True);
    switch (status) {
        case fmi2OK:
            _curComTime += _stepSize;
            return true;
            break;
        case fmi2Discard:
            _logger->Log(this, "Step failed. Terminating the FMI co-sosimulation", Smp::Services::ILogger::LMK_Warning);
            terminate();
            break;
        case fmi2Pending:
            /* TODO handle asynchronus fmi2DoStep */
            _logger->Log(this, "Asynchronous step of FMI models is not yet implemented!",
                         Smp::Services::ILogger::LMK_Warning);
            break;
        case fmi2Warning:
            _logger->Log(this, "The FMI model raised a warning", Smp::Services::ILogger::LMK_Warning);
            break;
        case fmi2Error:
        case fmi2Fatal:
            const auto msg("An error occured during the step of the FMI model");
            _logger->Log(this, msg, Smp::Services::ILogger::LMK_Error);
            throw new simdeck::Exception(this, msg);
            break;
    }

    return false;
}
// ..........................................................
void FMILoad::terminate() {
    /* Get variables */
    /* Call fmi2FreeInstance */
    if (_fmiObj != nullptr) {
        getFn(FreeInstance) FreeInstance(_fmiObj);
        _fmiObj == nullptr;
    }
}
// ..........................................................
void FMILoad::reset() {
    /* Call fmi2Reset */
    getFn(Reset) Reset(_fmiObj);

    /* Setup */
    setup();
}
// ..........................................................
} /* namespace fmi */
} /* namespace simphonie */
