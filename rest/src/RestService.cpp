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

#include "simdeck/ExInvalidParent.hpp"
#include "wfrest/Json.h"

namespace simphonie {
namespace rest {

using namespace wfrest;
// --------------------------------------------------------------------
// ..........................................................
RestService::RestService(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent) : Parent(name, descr, parent) {
    _sim = dynamic_cast<Smp::ISimulator*>(parent);
    if (_sim != nullptr) {
        std::string root = "/";
        root += _sim->GetName();
        _server.GET(root, [&](const HttpReq* req, HttpResp* resp) { this->getSimulator(req, resp); });
        _server.start(8080);
    }
    else {
        simdeck::ExInvalidParent ex(this, parent, nullptr);
        // TODO need a way to add info on exception...
        //        ex.setDescription("Parent is not a Smp::ISimulator");
        throw ex;
    }
}
// ..........................................................
RestService::~RestService() {
    _server.stop();
}
// --------------------------------------------------------------------
// ..........................................................
void RestService::getSimulator(const HttpReq* req, HttpResp* resp) {
    Json json;
    json["name"] = _sim->GetName();
    //    json["state"]=_sim->GetState();
    Json::Array children;
    for (auto c : *(_sim->GetContainers())) {
        for (auto comp : *(c->GetComponents())) {
            children.push_back(comp->GetName());
        }
    }
    json["children"] = children;
    resp->Json(json);
}

}  // namespace rest
}  // namespace simphonie
