/*
 * @file RestService.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_rest_RestService_HPP__
#define __simphonie_rest_RestService_HPP__

#include "Smp/ISimulator.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simdeck/Service.hpp"
#include "wfrest/HttpServer.h"
#include "wfrest/Json.h"

namespace simphonie {
namespace rest {

using namespace wfrest;

class RestService : public simdeck::Service {
public:
    RestService(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    ~RestService();

private:
    void connect();
    Json::Object parseTimestamp() const;
    Json::Object parseUuid(const Smp::Uuid& uuid) const;
    Json::Object parseType(const Smp::Publication::IType* type) const;
    template <typename T>
    Json::Object parseKind(const T& kind) const;
    Json::Object parseField(const Smp::IField* field) const;
    Json::Object parseEP(const Smp::IEntryPoint* ep) const;
    Json::Array parseFields(const Smp::IComponent* Component) const;
    Json::Array parseEPs(const Smp::IComponent* Component) const;
    Json::Object parseComponent(const Smp::IComponent* component) const;
    Json::Array parseContainer(const Smp::IContainer* container) const;
    void getSimulator(const HttpReq* req, HttpResp* resp) const;
    void getState(const HttpReq* req, HttpResp* resp) const;
    void getField(const HttpReq* req, HttpResp* resp) const;
    void getEP(const HttpReq* req, HttpResp* resp) const;
    void getComponent(const HttpReq* req, HttpResp* resp) const;
    void getContainer(const HttpReq* req, HttpResp* resp) const;

    HttpServer _server;
    const Smp::ISimulator* _sim;
    Smp::Services::ITimeKeeper* _tk;
};

} /* namespace rest */
} /* namespace simphonie */
#endif /* __simphonie_rest_RestService_HPP__ */
