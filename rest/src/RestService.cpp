/*
 * @file RestService.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/rest/RestService.hpp"
#include "Smp/IEntryPointPublisher.h"
#include "Smp/Publication/IType.h"
#include "Smp/ViewKind.h"
#include "simdeck/ExInvalidParent.hpp"

namespace simphonie {
namespace rest {

using namespace wfrest;

RestService::RestService(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simdeck::Service(name, descr, parent), _sim(dynamic_cast<Smp::ISimulator*>(parent)) {
    if (_sim != nullptr) {
        std::string root = "/";
        root += _sim->GetName();
        _server.GET(root, [=](const HttpReq* req, HttpResp* resp) { this->getSimulator(req, resp); });
        _server.GET(root + "/state", [=](const HttpReq* req, HttpResp* resp) { this->getState(req, resp); });
        // _server.GET(root + "/content/*/fields/{name}", [=](const HttpReq* req, HttpResp* resp) { this->getField(req,
        // resp); }); _server.GET(root + "/content/*/entrypoints/{name}", [=](const HttpReq* req, HttpResp* resp) {
        // this->getEP(req, resp); }); _server.GET(root + "/content/*/children/{name}", [=](const HttpReq* req,
        // HttpResp* resp) { this->getComponent(req, resp); }); _server.GET(root + "/content/*/{name}/children",
        // [=](const HttpReq* req, HttpResp* resp) { this->getContainer(req, resp); });
        _server.start(8080); /* TODO add config params */
    }
    else {
        simdeck::ExInvalidParent ex(this, parent, nullptr);
        // TODO need a way to add info on exception...
        //        ex.setDescription("Parent is not a Smp::ISimulator");
        throw ex;
    }
}

RestService::~RestService() {
    _server.stop();
}

void RestService::connect() {
    _tk = _sim->GetTimeKeeper();
}

Json::Object RestService::parseTimestamp() const {
    return Json::Object{
        {"zulu", _tk->GetZuluTime()},
        {"simulation", _tk->GetSimulationTime()},
        {"epoch", _tk->GetEpochTime()},
        {"mission", _tk->GetMissionTime()},
    };
}

Json::Object RestService::parseUuid(const Smp::Uuid& uuid) const {
    std::string name;
    {
        std::ostringstream oss;
        oss << uuid;
        name = oss.str();
    }
    return Json::Object{
        {"uint32", uuid.Data1},
        {"string", name},
    };
}

Json::Object RestService::parseType(const Smp::Publication::IType* type) const {
    return Json::Object{
        {"uuid", parseUuid(type->GetUuid())},
        {"primitive", parseKind(type->GetPrimitiveTypeKind())},
    };
}

template <typename T>
Json::Object RestService::parseKind(const T& kind) const {
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

Json::Object RestService::parseField(const Smp::IField* field) const {
    return Json::Object{
        {"name", field->GetName()},      {"viewKind", static_cast<Smp::Int32>(field->GetView())},
        {"isState", field->IsState()},   {"isInput", field->IsInput()},
        {"isOutput", field->IsOutput()}, {"type", parseType(field->GetType())},
    };
}

Json::Object RestService::parseEP(const Smp::IEntryPoint* ep) const {
    return Json::Object{
        {"name", ep->GetName()},
        {"description", ep->GetDescription()},
    };
}

Json::Array RestService::parseFields(const Smp::IComponent* component) const {
    Json::Array json;
    for (const auto field : *(component->GetFields())) {
        json.push_back(parseField(field));
    }
    return json;
}

Json::Array RestService::parseEPs(const Smp::IComponent* component) const {
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

Json::Object RestService::parseComponent(const Smp::IComponent* component) const {
    Json::Object json;
    json.push_back("name", component->GetName());
    json.push_back("description", component->GetDescription());
    json.push_back("uuid", parseUuid(component->GetUuid()));
    json.push_back("state", parseKind(component->GetState()));
    {
        const auto fields = parseFields(component);
        if (fields.size() > 0) {
            json.push_back("Fields", fields);
        }
    }
    {
        const auto eps = parseEPs(component);
        if (eps.size() > 0) {
            json.push_back("EntryPoints", eps);
        }
    }
    {
        const auto composite = dynamic_cast<const Smp::IComposite*>(component);
        if (composite) {
            for (const auto cont : *(composite->GetContainers())) {
                json.push_back(cont->GetName(), parseContainer(cont));
            }
        }
    }
    return json;
}

Json::Array RestService::parseContainer(const Smp::IContainer* container) const {
    Json::Array json;
    for (const auto comp : *(container->GetComponents())) {
        json.push_back(parseComponent(comp));
    }
    return json;
}

void RestService::getState(const HttpReq* req, HttpResp* resp) const {
    auto json = parseKind(_sim->GetState());
    json.push_back("timestamp", parseTimestamp());
    resp->Json(json);
}

void RestService::getSimulator(const HttpReq* req, HttpResp* resp) const {
    Json::Object json;
    json.push_back("name", _sim->GetName());
    json.push_back("description", _sim->GetDescription());
    json.push_back("state", parseKind(_sim->GetState()));
    for (const auto cont : *(_sim->GetContainers())) {
        json.push_back(cont->GetName(), parseContainer(cont));
    }
    json.push_back("timestamp", parseTimestamp());
    resp->Json(json);
}

} /* namespace rest */
} /* namespace simphonie */
