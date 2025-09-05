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
#include "Smp/Uuid.h"
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
    _receiver = receiver;
    _receiver->PublishField("path", "Path to the fmu file (zipped or not)", &_path, simdeck::StringType::UuidString,
                            Smp::ViewKind::VK_All, false, true, false);
    _receiver->PublishField("instanceName", "Name of the instance (c.f. fmi2Instantiate's documentation)",
                            &_instanceName, simdeck::StringType::UuidString, Smp::ViewKind::VK_All, false, true, false);
    _receiver->PublishField("tolerance",
                            "Tolerance of the error estimation of the communication interval (c.f. "
                            "fmi2SetupExperiment's documentation): -1.0 for the default value",
                            &_tolerance, Smp::ViewKind::VK_All, false, true, false);
    _receiver->PublishField(
        "startTime",
        "Staring time of the simulation (c.f. fmi2SetupExperiment's documentation): -1.0 for the default value",
        &_startTime, Smp::ViewKind::VK_All, false, true, false);
    _receiver->PublishField(
        "stopTime",
        "Stopping time of the simulation (c.f. fmi2SetupExperiment's documentation): -1.0 for the default value",
        &_stopTime, Smp::ViewKind::VK_All, false, true, false);
    _receiver->PublishField(
        "stepSize", "Step size of doStep function (c.f. fmi2DoStep's documentation): -1.0 for the default value",
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
    /* check if it is a co-simulation FMU */
    {
        auto ctx = xmlXPathNewContext(doc);
        if (ctx == nullptr) {
            auto msg("Unable to check if it is a co-simulation model: XML XPath context cannot be created");
            _logger->Log(this, msg, Smp::Services::ILogger::LMK_Error);
            throw new simdeck::ExInvalidFile(this, _path.c_str(), msg);
        }
        else {
            auto cosim =
                xmlXPathEvalExpression(reinterpret_cast<const xmlChar*>("/fmiModelDescription/CoSimulation"), ctx);
            if (cosim == nullptr) {
                auto msg("The provided FMI model is not a co-simulation");
                _logger->Log(this, msg, Smp::Services::ILogger::LMK_Error);
                throw new simdeck::ExInvalidFile(this, _path.c_str(), msg);
            }
            xmlXPathFreeObject(cosim);
        }
        xmlXPathFreeContext(ctx);
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
            _logger->Log(this, "No default experiment for the FMI model: XML XPath context cannot be created",
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
                    auto res = reinterpret_cast<char*>(xmlGetProp(node, reinterpret_cast<const xmlChar*>("startTime")));
                    if (res != nullptr) {
                        _startTime = std::stod(res);
                    }
                    xmlFree(res);
                }
                if (_stopTime < 0.0) {
                    auto res = reinterpret_cast<char*>(xmlGetProp(node, reinterpret_cast<const xmlChar*>("stopTime")));
                    if (res != nullptr) {
                        _stopTime = std::stod(res);
                    }
                    xmlFree(res);
                }
                if (_tolerance < 0.0) {
                    auto res = reinterpret_cast<char*>(xmlGetProp(node, reinterpret_cast<const xmlChar*>("tolerance")));
                    if (res != nullptr) {
                        _tolerance = std::stod(res);
                    }
                    xmlFree(res);
                }
                if (_stepSize < 0.0) {
                    auto res = reinterpret_cast<char*>(xmlGetProp(node, reinterpret_cast<const xmlChar*>("stepSize")));
                    if (res != nullptr) {
                        _stepSize = std::stod(res);
                    }
                    xmlFree(res);
                }
                xmlXPathFreeObject(defaultExp);
            }
        }
        xmlXPathFreeContext(ctx);
    }
    /* publish fields */
    for (auto cur = root->children; cur != nullptr; cur = cur->next) {
        if (cur->type == XML_ELEMENT_NODE
            && xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("ModelVariables")) == 0) {
            for (auto var = cur->children; var != nullptr; var = var->next) {
                if (cur->type == XML_ELEMENT_NODE
                    && xmlStrcmp(var->name, reinterpret_cast<const xmlChar*>("ScalarVariable")) == 0) {
                    /* read from xml */
                    auto name = reinterpret_cast<char*>(xmlGetProp(var, reinterpret_cast<const xmlChar*>("name")));
                    if (name == nullptr) {
                        _logger->Log(this, "A variable cannot be retrieve as it does not have a name",
                                     Smp::Services::ILogger::LMK_Warning);
                        continue;
                    }
                    auto description =
                        reinterpret_cast<char*>(xmlGetProp(var, reinterpret_cast<const xmlChar*>("description")));
                    auto ref =
                        reinterpret_cast<char*>(xmlGetProp(var, reinterpret_cast<const xmlChar*>("valueReference")));
                    if (ref == nullptr) {
                        std::ostringstream msg;
                        msg << "Variable named '" << name
                            << "' cannot be retrieve has it does not have a valueRefence attribute";
                        _logger->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Warning);
                        xmlFree(name);
                        xmlFree(description);
                        continue;
                    }
                    auto causality =
                        reinterpret_cast<char*>(xmlGetProp(var, reinterpret_cast<const xmlChar*>("causality")));
                    auto variability =
                        reinterpret_cast<char*>(xmlGetProp(var, reinterpret_cast<const xmlChar*>("variability")));
                    auto initial =
                        reinterpret_cast<char*>(xmlGetProp(var, reinterpret_cast<const xmlChar*>("initial")));
                    /* normalize and check name */
                    /* TODO */
                    for (auto c = name; *c != '\0'; ++c) {
                        if (*c == '(' || *c == ')') {
                            *c = '_';
                        }
                    }
                    if (_receiver->GetField(name) != nullptr) {
                        std::ostringstream msg;
                        msg << "Variable with the normalized name '" << name << "' already exists";
                        _logger->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Warning);
                        xmlFree(name);
                        xmlFree(description);
                        xmlFree(ref);
                        xmlFree(causality);
                        continue;
                    }
                    /* deduce its causality */
                    Variable::Causality caus;
                    if (causality == nullptr)
                        caus = Variable::Causality::LOCAL;
                    else if (std::strcmp(causality, "parameter") == 0)
                        caus = Variable::Causality::PARAMETER;
                    else if (std::strcmp(causality, "calculatedParameter") == 0)
                        caus = Variable::Causality::CALCULATED_PARAMETER;
                    else if (std::strcmp(causality, "input") == 0)
                        caus = Variable::Causality::INPUT;
                    else if (std::strcmp(causality, "output") == 0)
                        caus = Variable::Causality::OUTPUT;
                    else if (std::strcmp(causality, "local") == 0)
                        caus = Variable::Causality::LOCAL;
                    else if (std::strcmp(causality, "independent") == 0)
                        caus = Variable::Causality::INDEPENDENT;
                    else {
                        std::ostringstream msg;
                        msg << "The variable with the normalized name '" << name << " has a wrong causality: '"
                            << causality << "'";
                        _logger->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Warning);
                        continue;
                    }
                    /* deduce its variability */
                    Variable::Variability vbly;
                    if (variability == nullptr)
                        vbly = Variable::Variability::CONTINUOUS;
                    else if (std::strcmp(variability, "constant") == 0)
                        vbly = Variable::Variability::CONSTANT;
                    else if (std::strcmp(variability, "fixed") == 0)
                        vbly = Variable::Variability::FIXED;
                    else if (std::strcmp(variability, "tunable") == 0)
                        vbly = Variable::Variability::TUNABLE;
                    else if (std::strcmp(variability, "discrete") == 0)
                        vbly = Variable::Variability::DISCRETE;
                    else if (std::strcmp(variability, "continuous") == 0)
                        vbly = Variable::Variability::CONTINUOUS;
                    else {
                        std::ostringstream msg;
                        msg << "The variable with the normalized name '" << name << "' has a wrong variability: '"
                            << variability << "'";
                        _logger->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Warning);
                        continue;
                    }
                    /* deduce its initial */
                    Variable::Initial init;
                    if (initial == nullptr) {
                        /* default value (page 48-49) */
                        if ((vbly == Variable::Variability::CONSTANT
                             && (caus == Variable::Causality::OUTPUT || caus == Variable::Causality::LOCAL))
                            || (caus == Variable::Causality::PARAMETER
                                && (vbly == Variable::Variability::FIXED || vbly == Variable::Variability::TUNABLE))) {
                            init = Variable::Initial::EXACT;
                        }
                        else if (((vbly == Variable::Variability::FIXED || vbly == Variable::Variability::TUNABLE)
                                  && (caus == Variable::Causality::LOCAL
                                      || caus == Variable::Causality::CALCULATED_PARAMETER))
                                 || ((vbly == Variable::Variability::DISCRETE
                                      || vbly == Variable::Variability::CONTINUOUS)
                                     && (caus == Variable::Causality::OUTPUT || caus == Variable::Causality::LOCAL))) {
                            init = Variable::Initial::CALCULATED;
                        }
                        else if ((vbly == Variable::Variability::CONTINUOUS && caus == Variable::Causality::INDEPENDENT)
                                 || ((vbly == Variable::Variability::DISCRETE
                                      || vbly == Variable::Variability::CONTINUOUS)
                                     && caus == Variable::Causality::INPUT)) {
                            init = Variable::Initial::UNDEFINED;
                        }
                        else {
                            std::ostringstream msg;
                            msg << "The variable with the normalized name '" << name
                                << "' likely have a wrong causality/variability pair, check "
                                   "https://fmi-standard.org/assets/releases/"
                                   "FMI_for_ModelExchange_and_CoSimulation_v2.0.p page 48-49 for more: variablity='"
                                << variability << "', causality='" << causality << "'";
                            _logger->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Warning);
                            continue;
                        }
                    }
                    else if (std::strcmp(initial, "exact") == 0)
                        init = Variable::Initial::EXACT;
                    else if (std::strcmp(initial, "approx") == 0)
                        init = Variable::Initial::APPROX;
                    else if (std::strcmp(initial, "calculated") == 0)
                        init = Variable::Initial::CALCULATED;
                    else {
                        std::ostringstream msg;
                        msg << "The variable with the normalized name '" << name << "' has a wrong initial attribute: '"
                            << initial << "'";
                        _logger->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Warning);
                        continue;
                    }
                    /* get its type */
                    auto type = var->children;
                    const auto typeName = reinterpret_cast<const char*>(type->name);
                    auto start = reinterpret_cast<char*>(xmlGetProp(type, reinterpret_cast<const xmlChar*>("start")));
                    void* ptr;
                    Smp::Uuid uuid;
                    /* TODO fix platform dependency */
                    if (std::strcmp(typeName, "Real")) {
                        const auto val = (start != nullptr) ? std::stod(start) : 0.0;
                        ptr = new fmi2Real(val);
                        uuid = Smp::Uuids::Uuid_Float64;
                    }
                    else if (std::strcmp(typeName, "Integer")) {
                        const auto val = (start != nullptr) ? std::stoi(start) : 0;
                        ptr = new fmi2Integer(val);
                        uuid = Smp::Uuids::Uuid_Int32;
                    }
                    else if (std::strcmp(typeName, "Boolean")) {
                        const auto val = (start != nullptr) ? (std::strcmp(start, "true") == 0) : false;
                        ptr = new fmi2Boolean(val);
                        uuid = Smp::Uuids::Uuid_Bool;
                    }
                    else if (std::strcmp(typeName, "String")) {
                        const auto val = (start != nullptr) ? start : "";
                        ptr = new fmi2String(val);
                        uuid = simdeck::StringType::UuidString;
                    }
                    /* add the variable to _vars */
                    auto field = dynamic_cast<Smp::ISimpleField*>(_receiver->PublishField(
                        name, (description != nullptr) ? description : "", ptr, uuid, Smp::ViewKind::VK_All, false,
                        caus == Variable::Causality::INPUT, caus == Variable::Causality::OUTPUT));
                    _vars.push_back(
                        Variable(static_cast<fmi2ValueReference>(std::stoul(ref)), caus, vbly, init, uuid, field));
                    /* free */
                    xmlFree(name);
                    xmlFree(description);
                    xmlFree(ref);
                    xmlFree(causality);
                    xmlFree(variability);
                    xmlFree(initial);
                }
            }
            break; /* we read the whole ModelVariables block */
        }
    }
    /* free xml's stuff */
    xmlFreeDoc(doc);
    xmlCleanupParser();
    /* build path to the main lib */
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

    /* Retrieve functions */
    getFn(DoStep) _doStepFn = DoStep;
    getFn(SetReal) _setRealFn = SetReal;
    getFn(SetInteger) _setIntegerFn = SetInteger;
    getFn(SetBoolean) _setBooleanFn = SetBoolean;
    getFn(SetString) _setStringFn = SetString;
    if (_doStepFn == nullptr || _setRealFn == nullptr || _setIntegerFn == nullptr || _setBooleanFn == nullptr
        || _setStringFn == nullptr) {
        const auto msg(
            "Unable to find at least one of those required function: fmi2DoStep, fmi2SetReal, fmi2SetInteger, "
            "fmi2SetBoolean, fmi2SetString");
        _logger->Log(this, msg, Smp::Services::ILogger::LMK_Error);
        throw new simdeck::ExInvalidFile(this, _path.c_str(), msg);
    }

    /* Setup */
    setup();
}
// ..........................................................
void FMILoad::setup() {
    /**
     * 1:
     * - set variables with variablility!=constant and (initial=exact or initial=approx)
     * - call fmi2SetupExperiment
     */
    {
        std::vector<Variable*> vars;
        for (auto& var : _vars) {
            if (var.variability != Variable::Variability::CONSTANT
                && (var.initial == Variable::Initial::EXACT || var.initial == Variable::Initial::APPROX)) {
                vars.push_back(&var);
            }
        }
        setVariables(vars);
    }
    getFn(SetupExperiment)
        SetupExperiment(_fmiObj, (_tolerance >= 0), _tolerance, _startTime, (_stopTime >= 0), _stopTime);
    /* 2: set variables with variablility!=constant and (initial=exact or causality=input) */
    {
        std::vector<Variable*> vars;
        for (auto& var : _vars) {
            if (var.variability != Variable::Variability::CONSTANT
                && (var.initial == Variable::Initial::EXACT || var.causality == Variable::Causality::INPUT)) {
                vars.push_back(&var);
            }
        }
        setVariables(vars);
    }
    /* 3: call fmi2EnterInitializationMode */
    getFn(EnterInitializationMode) EnterInitializationMode(_fmiObj);
    /**
     * 4:
     * - set variables with causability=input or (causability=parameter and variability=tunable)
     * - set variables with variablility!=constant and initial=exact
     */
    {
        std::vector<Variable*> vars;
        for (auto& var : _vars) {
            if ((var.variability != Variable::Variability::CONSTANT && var.initial == Variable::Initial::EXACT)
                || (var.causality != Variable::Causality::INPUT
                    || (var.causality != Variable::Causality::PARAMETER
                        && var.variability != Variable::Variability::TUNABLE))) {
                vars.push_back(&var);
            }
        }
        setVariables(vars);
    }
    /* 5: call fmi2ExitInitializationMode */
    getFn(ExitInitializationMode) ExitInitializationMode(_fmiObj);

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
void FMILoad::setVariables(std::vector<Variable*>& vars) {
    /* Real */
    // {
    //     std::vector<fmi2ValueReference> refs;
    //     std::vector<fmi2Real> vals;
    //     for (auto var : vars) {
    //         if (var->uuid == Smp::Uuids::Uuid_Float64) {
    //             refs.push_back(var->ref);
    //             vals.push_back(var->field->GetValue());
    //         }
    //     }
    //     _setRealFn(_fmiObj, refs.data(), refs.size(), vals.data());
    // }
    /* Integer */
    // {
    //     std::vector<fmi2ValueReference> refs;
    //     std::vector<fmi2Integer> vals;
    //     for (auto var : vars) {
    //         if (var->uuid == Smp::Uuids::Uuid_Int32) {
    //             refs.push_back(var->ref);
    //             vals.push_back(var->field->GetValue());
    //         }
    //     }
    //     _setIntegerFn(_fmiObj, refs.data(), refs.size(), vals.data());
    // }
    /* Boolean */
    // {
    //     std::vector<fmi2ValueReference> refs;
    //     std::vector<fmi2Boolean> vals;
    //     for (auto var : vars) {
    //         if (var->uuid == Smp::Uuids::Uuid_Bool) {
    //             refs.push_back(var->ref);
    //             vals.push_back(var->field->GetValue());
    //         }
    //     }
    //     _setBooleanFn(_fmiObj, refs.data(), refs.size(), vals.data());
    // }
    /* String */
    // {
    //     std::vector<fmi2ValueReference> refs;
    //     std::vector<fmi2String> vals;
    //     for (auto var : vars) {
    //         if (var->uuid == simdeck::StringType::UuidString) {
    //             refs.push_back(var->ref);
    //             vals.push_back(var->field->GetValue()); /* TODO allowed? */
    //         }
    //     }
    //     _setStringFn(_fmiObj, refs.data(), refs.size(), vals.data());
    // }
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
            /* TODO get vars? */
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
// --------------------------------------------------------------------
// ..........................................................
FMILoad::Variable::Variable(const fmi2ValueReference ref, const Causality causality, const Variability variabliity,
                            const Initial initial, const Smp::Uuid uuid, Smp::ISimpleField* field)
    : ref(ref), causality(causality), variability(variability), initial(initial), uuid(uuid), field(field){};
} /* namespace fmi */
} /* namespace simphonie */
