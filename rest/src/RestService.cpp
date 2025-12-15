/*
 * @file RestService.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/rest/RestService.hpp"

#include <string>
#include <utility>

#include "Smp/IEntryPointPublisher.h"
#include "Smp/Publication/IType.h"
#include "Smp/Services/IEventManager.h"
#include "Smp/SimulatorStateKind.h"
#include "Smp/ViewKind.h"
#include "simdeck/ExInvalidParent.hpp"
#include "simdeck/StringType.hpp"
#include "wfrest/ErrorCode.h"
#include "wfrest/HttpMsg.h"

#define ON_SIM_EXECUTING "onsimexecuting"
#define ON_SIM_LEAVEEXEC "onsimleavingexec"

namespace simphonie {
namespace rest {

using namespace wfrest;

const char* _hexDigit = "0123456789abcdef";

RestService::RestService(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simdeck::Service(name, descr, parent),
      _family(AF_INET),
      _host("localhost"),
      _port(8080) {

    addEP(ON_SIM_EXECUTING, "Internal use only", this, &RestService::onSimExecuting);
    addEP(ON_SIM_LEAVEEXEC, "Internal use only", this, &RestService::onSimLeavingExec);
}

RestService::~RestService() {
    _server.stop();
}


RestService::FieldHandler::FieldHandler(Smp::IField* field, Smp::Services::IScheduler* scheduler, bool* simIsRunning,
                                        Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simdeck::Object(name, descr, parent),
      _field(field),
      _simplefield(dynamic_cast<Smp::ISimpleField*>(_field)),
      _simplearrayfield(dynamic_cast<Smp::ISimpleArrayField*>(_field)),
      _updated(false),
      _scheduler(scheduler),
      _simIsRunning(simIsRunning) {
    _updateEP = addEP("update", "Update the value of the handled fields.", this, &RestService::FieldHandler::update);
}

void RestService::FieldHandler::update() {
    {
        std::lock_guard<std::mutex> lock(_bufMutex);

        /* store the new value */
        if (!_buf.empty()) {
            _field->Restore(this);
            _buf.clear(); /* to make sure it is empty */
        }

        /* retrieve the current bin value it it is not a simple one */
        if (!_simplefield && !_simplearrayfield) {
            _field->Store(this);
            _bin = std::string(_buf.data(), _buf.size());
            _buf.clear();
        }
    }

    std::unique_lock<std::mutex> lock(_updateMutex);

    /* retrieve the current anysimple values */
    _anysimples.clear();
    if (_simplefield) {
        _anysimples.push_back(_simplefield->GetValue());
    }
    if (_simplearrayfield) {
        const auto size = _simplearrayfield->GetSize();
        _anysimples.resize(size);
        _simplearrayfield->GetValues(size, _anysimples.data());
    }

    /* wake up waiting threads */
    _updated = true;
    _updatedCovar.notify_all();
}

void RestService::FieldHandler::scheduleUpdate() {
    if (*_simIsRunning) {
        _scheduler->AddImmediateEvent(_updateEP);
    }
    else {
        update();
    }
}

std::vector<std::string> RestService::FieldHandler::getValue() {
    /* schedule the update EP */
    scheduleUpdate();

    /* wait for the update */
    std::unique_lock<std::mutex> lock(_updateMutex);
    _updatedCovar.wait(lock, [&] { return _updated; });

    /* retrieve values */
    std::vector<std::string> value;
    for (const auto anysimple : _anysimples) {
        std::ostringstream oss;
        oss << anysimple;
        value.push_back(oss.str());
    }
    if (value.size() == 0) {
        /* if there is no simple value, we at least return the binary one */
        value.push_back(_bin);
    }
    return value;
}

void RestService::FieldHandler::setBinValue(const std::string& value) {
    std::lock_guard<std::mutex> lock(_bufMutex);
    _buf = std::vector<char>(value.begin(), value.end());
    scheduleUpdate();
}

void RestService::FieldHandler::Store(const Smp::Void* address, Smp::UInt64 size) {
    auto addr = reinterpret_cast<const uint8_t*>(address);
    for (Smp::UInt64 i = 0; i < size; ++i) {
        _buf.push_back(_hexDigit[(addr[i] & 0xF0) >> 4]);
        _buf.push_back(_hexDigit[addr[i] & 0x0F]);
    }
}

void RestService::FieldHandler::Restore(Smp::Void* address, Smp::UInt64 size) {
    auto addr = reinterpret_cast<uint8_t*>(address);
    for (Smp::UInt64 i = 0; i < size; ++i) {
        const auto high = getHexPos(_buf.front());
        _buf.erase(_buf.begin());
        const auto low = getHexPos(_buf.front());
        _buf.erase(_buf.begin());
        addr[i] = (high << 4) | low;
    }
}

void RestService::publish(Smp::IPublication* receiver) {
    receiver->PublishField("family", "IP family.", &_family, Smp::ViewKind::VK_All, false, true, false);
    receiver->PublishField("host", "Host.", &_host, simdeck::StringType::UuidString, Smp::ViewKind::VK_All, false, true,
                           false);
    receiver->PublishField("port", "Port.", &_port, Smp::ViewKind::VK_All, false, true, false);
}

void RestService::connect() {
    _sim=getSimulator();
    if (_sim != nullptr) {
        std::string root = "/api/v1/";
        root += _sim->GetName();
        _server.GET(root, [=](const HttpReq* req, HttpResp* resp) { this->getSimulatorStatus(req, resp); });
        _server.GET(root + "/state", [=](const HttpReq* req, HttpResp* resp) { this->getState(req, resp); });
        _server.GET(root + "/*", [=](const HttpReq* req, HttpResp* resp) { this->defaultGetHandler(req, resp); });
        _server.POST(root + "/state", [=](const HttpReq* req, HttpResp* resp) { this->postState(req, resp); });
        _server.POST(root + "/scheduleQueue",
                     [=](const HttpReq* req, HttpResp* resp) { this->postScheduleQueue(req, resp); });
        _server.POST(root + "/*", [=](const HttpReq* req, HttpResp* resp) { this->defaultPostHandler(req, resp); });
    }
    {
        std::lock_guard<std::mutex> lock(_simIsRunningMutex);
        _simIsRunning = (_sim->GetState() == Smp::SimulatorStateKind::SSK_Executing);
    }
    _timeKeeper = _sim->GetTimeKeeper();
    _resolver = _sim->GetResolver();
    _scheduler = _sim->GetScheduler();
    auto eventManager=_sim->GetEventManager();
    eventManager->Subscribe(Smp::Services::IEventManager::SMP_EnterExecutingId,
                                       GetEntryPoint(ON_SIM_EXECUTING));
    eventManager->Subscribe(Smp::Services::IEventManager::SMP_EnterExecutingId,
                                       GetEntryPoint(ON_SIM_LEAVEEXEC));
    _server.start(_family, _host.c_str(), _port);
}

void RestService::setupResponse(HttpResp* resp) {
    resp->set_header_pair("Access-Control-Origin", "*");
    resp->set_header_pair("Access-Control-Allow-Origin", "*");
    resp->set_header_pair("Access-Control-Allow-Methods", "GET, POST");
    resp->set_header_pair("Access-Control-Allow-Headers", "Content-Type");
}

std::string RestService::extractLastElemPath(std::string& path) {
    const auto pos = path.rfind('/');
    if (pos == std::string::npos) {
        const auto res = path;
        path = "";
        return res;
    }
    const auto res = path.substr(pos + 1);
    path = path.substr(0, pos);
    return res;
}

Json::Object RestService::parseTimestamp() const {
    return Json::Object{
        {"zulu", _timeKeeper->GetZuluTime()},
        {"simulation", _timeKeeper->GetSimulationTime()},
        {"epoch", _timeKeeper->GetEpochTime()},
        {"mission", _timeKeeper->GetMissionTime()},
    };
}

Json::Object RestService::parseType(const Smp::Publication::IType* type) {
    std::string uuid;
    {
        std::ostringstream oss;
        oss << type->GetUuid();
        uuid = oss.str();
    }
    return Json::Object{
        {"uuid", uuid},
        {"primitive", parseKind(type->GetPrimitiveTypeKind())},
    };
}

template <typename T>
Json::Object RestService::parseKind(const T& kind) {
    std::string name;
    {
        std::ostringstream oss;
        oss << kind;
        name = oss.str();
    }
    return Json::Object{
        {"id", static_cast<Smp::Int32>(kind)},
        {"name", name},
    };
}

Json::Object RestService::parseField(Smp::IField* field) {
    Json::Object json{
        {"name", field->GetName()},
        {"description", field->GetDescription()},
        {"viewKind", static_cast<Smp::Int32>(field->GetView())},
        {"isState", field->IsState()},
        {"isInput", field->IsInput()},
        {"isOutput", field->IsOutput()},
        {"type", parseType(field->GetType())},
    };
    {
        std::vector<std::string> value;
        {
            std::lock_guard<std::mutex> lock(_simIsRunningMutex);
            value = FieldHandler(field, _scheduler, &_simIsRunning, "FieldHandler", "", this).getValue();
        }
        if (value.size() == 1) {
            json.push_back("value", value.front());
        }
        else {
            Json::Array arr;
            for (const auto& s : value) {
                arr.push_back(s);
            }
            json.push_back("value", arr);
        }
    }
    return json;
}

Json::Object RestService::parseEP(const Smp::IEntryPoint* ep) {
    return Json::Object{
        {"name", ep->GetName()},
        {"description", ep->GetDescription()},
    };
}

Json::Array RestService::parseFields(const Smp::IComponent* component) {
    Json::Array json;
    for (auto field : *(component->GetFields())) {
        json.push_back(parseField(field));
    }
    return json;
}

Json::Array RestService::parseEPs(const Smp::IComponent* component) {
    const auto epp = dynamic_cast<const Smp::IEntryPointPublisher*>(component);
    Json::Array json;
    if (!epp) {
        return json;
    }
    for (const auto ep : *(epp->GetEntryPoints())) {
        json.push_back(parseEP(ep));
    }
    return json;
}

Json::Object RestService::parseComponent(const Smp::IComponent* component, bool recursive) {
    Json::Object json;
    json.push_back("name", component->GetName());
    json.push_back("description", component->GetDescription());
    {
        std::ostringstream oss;
        oss << component->GetUuid();
        json.push_back("uuid", oss.str());
    }
    json.push_back("state", parseKind(component->GetState()));
    if (recursive) {
        const auto fields = parseFields(component);
        if (fields.size() > 0) {
            json.push_back("fields", fields);
        }
    }
    else {
        Json::Array arr;
        for (const auto fld : *(component->GetFields())) {
            arr.push_back(fld->GetName());
        }
        json.push_back("fields", arr);
    }
    if (recursive) {
        const auto eps = parseEPs(component);
        if (eps.size() > 0) {
            json.push_back("entrypoints", eps);
        }
    }
    else {
        Json::Array arr;
        const auto epp = dynamic_cast<const Smp::IEntryPointPublisher*>(component);
        if (epp) {
            for (const auto fld : *(epp->GetEntryPoints())) {
                arr.push_back(fld->GetName());
            }
        }
        json.push_back("entrypoints", arr);
    }
    {
        const auto composite = dynamic_cast<const Smp::IComposite*>(component);
        if (composite) {
            if (recursive) {
                for (const auto cont : *(composite->GetContainers())) {
                    json.push_back(cont->GetName(), parseContainer(cont, recursive));
                }
            }
            else {
                Json::Array arr;
                for (const auto cont : *(composite->GetContainers())) {
                    for (const auto comp : *(cont->GetComponents())) {
                        arr.push_back(comp->GetName());
                    }
                    json.push_back(cont->GetName(), arr);
                    arr.clear();
                }
            }
        }
    }
    return json;
}

Json::Array RestService::parseContainer(const Smp::IContainer* container, bool recursive) {
    Json::Array json;
    for (const auto comp : *(container->GetComponents())) {
        json.push_back(parseComponent(comp, recursive));
    }
    return json;
}

void RestService::getState(const HttpReq* req, HttpResp* resp) const {
    setupResponse(resp);
    auto json = parseKind(_sim->GetState());
    json.push_back("timestamp", parseTimestamp());
    resp->Json(json);
}

void RestService::postScheduleQueue(const HttpReq* req, HttpResp* resp) {
    setupResponse(resp);
    const auto json = Json::parse(req->body());
    /* req->json() is better but it requires the content-type to be set to app/json */
    if (!json.is_valid()) {
        resp->String("Invalid json.");
        resp->set_status_code("422");
        return;
    }

    Smp::IEntryPoint* entrypoint;
    try {
        const auto path = json["entrypoint"].get<std::string>();
        const auto obj = _resolver->ResolveAbsolute(path.c_str());
        entrypoint = dynamic_cast<Smp::IEntryPoint*>(obj);
        if (!entrypoint) {
            throw std::exception();
        }
    }
    catch (...) {
        resp->String("Invalid entrypoint.");
        resp->set_status_code("422");
        return;
    }

    if (json.has("immediate") && json["immediate"].get<Smp::Bool>()) {
        _scheduler->AddImmediateEvent(entrypoint);
        return;
    }

    Smp::Duration period = 0;
    if (json.has("cycleTime_s")) {
        period = json["cycleTime_s"].get<Smp::Duration>() * 1000000000ULL;
    }
    else {
        if (json.has("cycleTime_ms")) {
            period = json["cycleTime_ms"].get<Smp::Duration>() * 1000000ULL;
        }
        else {
            if (json.has("cycleTime_us")) {
                period = json["cycleTime_us"].get<Smp::Duration>() * 1000ULL;
            }
            else {
                if (json.has("cycleTime_ns")) {
                    period = json["cycleTime_ns"].get<Smp::Duration>();
                }
            }
        }
    }

    Smp::Duration time = 0;
    if (json.has("offset_s")) {
        time = json["offset_s"].get<Smp::Duration>() * 1000000000ULL;
    }
    else {
        if (json.has("offset_ms")) {
            time = json["offset_ms"].get<Smp::Duration>() * 1000000ULL;
        }
        else {
            if (json.has("offset_us")) {
                time = json["offset_us"].get<Smp::Duration>() * 1000ULL;
            }
            else {
                if (json.has("offset_ns")) {
                    time = json["offset_ns"].get<Smp::Duration>();
                }
            }
        }
    }

    const auto repeat = json.has("repeat") ? json["repeat"].get<Smp::Int64>() : -1LL;

    try {
        _scheduler->AddSimulationTimeEvent(entrypoint, time, period, repeat);
    }
    catch (...) {
        resp->String("Scheduling failed.");
        resp->set_status_code("422");
    }
}

void RestService::postState(const HttpReq* req, HttpResp* resp) {
    setupResponse(resp);
    try {
        const auto json = Json::parse(req->body());
        /* req->json() is better but it requires the content-type to be set to app/json */
        if (!json.is_valid()) {
            throw std::exception();
        }
        const auto id = json["id"].get<Smp::Int32>();
        const auto ssk = static_cast<Smp::SimulatorStateKind>(id);
        switch (ssk) {
            case Smp::SimulatorStateKind::SSK_Building:
                _sim->Publish();
                _sim->Configure();
                break;
            case Smp::SimulatorStateKind::SSK_Initialising:
                _sim->Initialise();
                break;
            case Smp::SimulatorStateKind::SSK_Connecting:
                _sim->Connect();
                break;
            case Smp::SimulatorStateKind::SSK_Executing:
                _sim->Run();
                break;
            case Smp::SimulatorStateKind::SSK_Standby:
                _sim->Hold(true);
                break;
            case Smp::SimulatorStateKind::SSK_Storing: {
                const auto filename = json["filename"].get<std::string>();
                _sim->Store(filename.c_str());
            } break;
            case Smp::SimulatorStateKind::SSK_Restoring: {
                const auto filename = json["filename"].get<std::string>();
                _sim->Restore(filename.c_str());
            } break;
            case Smp::SimulatorStateKind::SSK_Exiting:
                _sim->Exit();
                break;
            case Smp::SimulatorStateKind::SSK_Aborting:
                _sim->Abort();
                break;
        }
    }
    catch (...) {
        resp->String("Invalid json.");
        resp->set_status_code("422");
    }
}

void RestService::getSimulatorStatus(const HttpReq* req, HttpResp* resp) {
    setupResponse(resp);
    Json::Object json;
    json.push_back("name", _sim->GetName());
    json.push_back("description", _sim->GetDescription());
    json.push_back("state", parseKind(_sim->GetState()));
    if (req->has_query("recursive") && req->query("recursive") == "true") {
        for (const auto cont : *(_sim->GetContainers())) {
            json.push_back(cont->GetName(), parseContainer(cont, true));
        }
    }
    else {
        Json::Array arr;
        for (const auto cont : *(_sim->GetContainers())) {
            for (const auto comp : *(cont->GetComponents())) {
                arr.push_back(comp->GetName());
            }
            json.push_back(cont->GetName(), arr);
            arr.clear();
        }
    }
    json.push_back("timestamp", parseTimestamp());
    resp->Json(json);
}

void RestService::defaultGetHandler(const HttpReq* req, HttpResp* resp) {
    setupResponse(resp);
    const auto path = "/" + req->match_path();

    auto objPath = path;
    const auto lastElem = extractLastElemPath(objPath);
    std::string prelastElem;
    if (lastElem != "fields" && lastElem != "entrypoints") {
        prelastElem = extractLastElemPath(objPath);
        if (prelastElem != "fields") {
            /* this is a component's path */
            objPath = path;
        }
    }

    const auto comp = dynamic_cast<Smp::IComponent*>(_resolver->ResolveAbsolute(objPath.c_str()));
    if (!comp) {
        const std::string msg = "the path '" + objPath + "' does not lead to a component.";
        resp->Error(ErrorCode::StatusNotFound, msg);
    }
    else {
        Json::Object json;
        if (lastElem == "fields") {
            json.push_back("fields", parseFields(comp));
        }
        else {
            if (lastElem == "entrypoints") {
                json.push_back("entrypoints", parseEPs(comp));
            }
            else {
                if (prelastElem == "fields") {
                    auto field = comp->GetField(lastElem.c_str());
                    json = parseField(field);
                }
                else {
                    json = parseComponent(comp, req->has_query("recursive") && req->query("recursive") == "true");
                }
            }
        }
        json.push_back("timestamp", parseTimestamp());
        resp->Json(json);
    }
}

void RestService::defaultPostHandler(const HttpReq* req, HttpResp* resp) {
    setupResponse(resp);
    Smp::IField* field;
    {
        const auto path = "/" + req->match_path();

        auto objPath = path;
        const auto lastElem = extractLastElemPath(objPath);
        const auto fields = extractLastElemPath(objPath);
        if (fields != "fields") {
            const std::string msg = "POST request does not lead to a field.";
            resp->Error(ErrorCode::StatusNotFound, msg);
            return;
        }
        const auto comp = dynamic_cast<Smp::IComponent*>(_resolver->ResolveAbsolute(objPath.c_str()));
        if (!comp) {
            const std::string msg = "the path '" + objPath + "' does not lead to a component.";
            resp->Error(ErrorCode::StatusNotFound, msg);
            return;
        }
        field = comp->GetField(lastElem.c_str());
        if (!field) {
            const std::string msg = "the field '" + lastElem + "' does not exist for the component '" + objPath + "'.";
            resp->Error(ErrorCode::StatusNotFound, msg);
            return;
        }
    }
    std::string value;
    {
        const auto json = Json::parse(req->body());
        /* req->json() is better but it requires the content-type to be set to app/json */
        if (!json.is_valid()) {
            resp->String("Invalid json found in the body request.");
            resp->set_status_code("422");
            return;
        }
        value = json["value"].get<std::string>();
    }
    {
        std::lock_guard<std::mutex> lock(_simIsRunningMutex);
        FieldHandler(field, _scheduler, &_simIsRunning, "FieldHandler", "", this).setBinValue(value);
    }
}

} /* namespace rest */
} /* namespace simphonie */
