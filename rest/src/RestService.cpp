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
#include "Smp/IEntryPointPublisher.h"
#include "Smp/Publication/IType.h"
#include "Smp/Services/IScheduler.h"
#include "Smp/SimulatorStateKind.h"
#include "Smp/ViewKind.h"
#include "simdeck/ExInvalidParent.hpp"
#include "simdeck/smpext/IObservableScheduler.hpp"
#include "simphonie/kern/Schedule.hpp" /* c.f. RestService::_compareSchedule::operator() */
#include "wfrest/ErrorCode.h"

namespace simphonie {
namespace rest {

using namespace wfrest;

const char* _hexDigit = "0123456789abcdef";

RestService::RestService(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simdeck::Service(name, descr, parent), _sim(dynamic_cast<Smp::ISimulator*>(parent)) {
    if (_sim != nullptr) {
        std::string root = "/";
        root += _sim->GetName();
        _server.GET(root, [=](const HttpReq* req, HttpResp* resp) { this->getSimulator(req, resp); });
        _server.GET(root + "/state", [=](const HttpReq* req, HttpResp* resp) { this->getState(req, resp); });
        _server.GET(root + "/scheduleQueue",
                    [=](const HttpReq* req, HttpResp* resp) { this->getScheduleQueue(req, resp); });
        _server.GET(root + "/*", [=](const HttpReq* req, HttpResp* resp) { this->defaultGetHandler(req, resp); });
        _server.POST(root + "/state", [=](const HttpReq* req, HttpResp* resp) { this->postState(req, resp); });
        _server.POST(root + "/scheduleQueue",
                     [=](const HttpReq* req, HttpResp* resp) { this->postScheduleQueue(req, resp); });
        _server.POST(root + "/*", [=](const HttpReq* req, HttpResp* resp) { this->defaultPostHandler(req, resp); });
        _server.start(8080); /* TODO add config params */
    }
    else {
        simdeck::ExInvalidParent ex(this, parent, nullptr);
        // TODO need a way to add info on exception...
        //        ex.setDescription("Parent is not a Smp::ISimulator");
        throw ex;
    }
    const auto schObsv = dynamic_cast<simdeck::smpext::IObservableScheduler*>(_sim->GetScheduler());
    if (schObsv) {
        schObsv->RegisterObserver(this);
    }
}

RestService::~RestService() {
    _server.stop();
    const auto schObsv = dynamic_cast<simdeck::smpext::IObservableScheduler*>(_sim->GetScheduler());
    if (schObsv) {
        schObsv->RemoveObserver(this);
    }
}

void RestService::notifyScheduled(const simdeck::smpext::ISchedule* event) {
    std::lock_guard<std::mutex> lock(_schdlMutex);
    _scheduleQueue.insert(event);
}

bool RestService::removeSchedule(Smp::Services::EventId eventId) {
    std::lock_guard<std::mutex> lock(_schdlMutex);
    for (auto it = _scheduleQueue.begin(); it != _scheduleQueue.end(); ++it) {
        if ((*it)->GetId() == eventId) {
            std::lock_guard<std::mutex> lock(_schdlMutex);
            _scheduleQueue.erase(it);
            return true;
        }
    }
    return false;
}

void RestService::_FieldHandler::update() {
    std::lock_guard<std::mutex> lock(_bufMtx);

    if (!_buf.empty()) {
        _field->Store(this);
        const auto value = std::string(_buf.data(), _buf.size());
        _buf.clear();
    }
    _field->Restore(this);
    _buf.clear(); /* to make sure it is empty */
    _bufCovar.notify_one();

    _anysimple = _simplefield->GetValue();
}

std::string RestService::_FieldHandler::getBinValue() {
    std::unique_lock<std::mutex> lock(_bufMtx);
    _bufCovar.wait(lock, [&] { return _buf.empty(); });

    _field->Store(this);
    const auto value = std::string(_buf.data(), _buf.size());
    _buf.clear();
    _bufCovar.notify_one();

    return value;
}

void RestService::_FieldHandler::setBinValue(const std::string& value) {
    std::lock_guard<std::mutex> lock(_bufMtx);
    _buf = std::vector<char>(value.begin(), value.end());
}

void RestService::_FieldHandler::Store(const Smp::Void* address, Smp::UInt64 size) {
    auto addr = reinterpret_cast<const uint8_t*>(address);
    for (Smp::UInt64 i = 0; i < size; ++i) {
        _buf.push_back(_hexDigit[(addr[i] & 0xF0) >> 4]);
        _buf.push_back(_hexDigit[addr[i] & 0x0F]);
    }
}

void RestService::_FieldHandler::Restore(Smp::Void* address, Smp::UInt64 size) {
    auto addr = reinterpret_cast<uint8_t*>(address);
    for (Smp::UInt64 i = 0; i < size; ++i) {
        const auto high = getHexPos(_buf.front());
        _buf.erase(_buf.begin());
        const auto low = getHexPos(_buf.front());
        _buf.erase(_buf.begin());
        addr[i] = (high << 4) | low;
    }
}

void RestService::connect() {
    _tk = _sim->GetTimeKeeper();
    _rslv = _sim->GetResolver();
    _schdl = _sim->GetScheduler();
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
        {"zulu", _tk->GetZuluTime()},
        {"simulation", _tk->GetSimulationTime()},
        {"epoch", _tk->GetEpochTime()},
        {"mission", _tk->GetMissionTime()},
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
    _FieldHandler fldhdl(field);
    Json::Object value{
        {"bin", fldhdl.getBinValue()},
    };
    if (fldhdl.isSimpleField()) {
        value.push_back("string", fldhdl.getStrValue());
    }
    return Json::Object{
        {"name", field->GetName()},
        {"viewKind", static_cast<Smp::Int32>(field->GetView())},
        {"isState", field->IsState()},
        {"isInput", field->IsInput()},
        {"isOutput", field->IsOutput()},
        {"type", parseType(field->GetType())},
        {"value", value},
    };
}

Json::Object RestService::parseEP(const Smp::IEntryPoint* ep) {
    return Json::Object{
        {"name", ep->GetName()},
        {"description", ep->GetDescription()},
    };
}

Json::Object RestService::parseSchedule(const simdeck::smpext::ISchedule* schedule) {
    Json::Object json{
        {"id", schedule->GetId()},
        {"name", schedule->GetName()},
        {"description", schedule->GetDescription()},
        {"time", schedule->GetTime()},
        {"period", schedule->GetPeriod()},
        {"activationCounter", schedule->GetActivationCounter()},
    };
    if (schedule->GetStartEventId() >= 0) {
        json.push_back("StartEventId", schedule->GetStartEventId());
    }
    if (schedule->GetStopEventId() >= 0) {
        json.push_back("StopEventId", schedule->GetStopEventId());
    }
    if (schedule->GetRepeat() >= 0) {
        json.push_back("repeat", schedule->GetRepeat());
    }
    return json;
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
        {
            const auto fields = parseFields(component);
            if (fields.size() > 0) {
                json.push_back("fields", fields);
            }
        }
        {
            const auto eps = parseEPs(component);
            if (eps.size() > 0) {
                json.push_back("entrypoints", eps);
            }
        }
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
    auto json = parseKind(_sim->GetState());
    json.push_back("timestamp", parseTimestamp());
    resp->Json(json);
}

void RestService::getScheduleQueue(const HttpReq* req, HttpResp* resp) {
    Json::Object json;
    for (const auto schedule : _scheduleQueue) {
        json["queue"].push_back(parseSchedule(schedule));
    }
    json.push_back("timestamp", parseTimestamp());
    resp->Json(json);
}

void RestService::postScheduleQueue(const HttpReq* req, HttpResp* resp) {
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
        const auto obj = _rslv->ResolveAbsolute(path.c_str());
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
        _schdl->AddImmediateEvent(entrypoint);
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

    const auto repeat = json.has("repetitions") ? json["repetitions"].get<Smp::Int64>() : -1LL;

    try {
        _schdl->AddSimulationTimeEvent(entrypoint, time, period, repeat);
    }
    catch (...) {
        resp->String("Scheduling failed.");
        resp->set_status_code("422");
    }
}

void RestService::postState(const HttpReq* req, HttpResp* resp) {
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

void RestService::getSimulator(const HttpReq* req, HttpResp* resp) {
    Json::Object json;
    json.push_back("name", _sim->GetName());
    json.push_back("description", _sim->GetDescription());
    json.push_back("state", parseKind(_sim->GetState()));
    if (req->has_query("recursive")) {
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

    const auto comp = dynamic_cast<Smp::IComponent*>(_rslv->ResolveAbsolute(objPath.c_str()));
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
                    json = parseComponent(comp, req->has_query("recursive"));
                }
            }
        }
        json.push_back("timestamp", parseTimestamp());
        resp->Json(json);
    }
}

void RestService::defaultPostHandler(const HttpReq* req, HttpResp* resp) {
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
        const auto comp = dynamic_cast<Smp::IComponent*>(_rslv->ResolveAbsolute(objPath.c_str()));
        if (!comp) {
            const std::string msg = "the path '" + objPath + "' does not lead to a component.";
            resp->Error(ErrorCode::StatusNotFound, msg);
            return;
        }
        field = comp->GetField(lastElem.c_str());
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
    _FieldHandler fldhdl(field);
    fldhdl.setBinValue(value);
}

/**
 * Dirty tricks to get the scheduler's comparaison function.
 * It may be cleaner to define a `virtual bool operator<(const ISchedule& other) const = 0;` within the
 * simdeck::smpext::ISchedule class
 */
bool RestService::_compareSchedule::operator()(const simdeck::smpext::ISchedule* a,
                                               const simdeck::smpext::ISchedule* b) const {
    return *reinterpret_cast<const simphonie::kern::Schedule*>(a)
           < *reinterpret_cast<const simphonie::kern::Schedule*>(b);
}

} /* namespace rest */
} /* namespace simphonie */
