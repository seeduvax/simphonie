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
#include "Smp/Services/IResolver.h"
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
    static std::string extractLastElemPath(std::string& path);
    Json::Object parseTimestamp() const;
    static Json::Object parseUuid(const Smp::Uuid& uuid);
    static Json::Object parseType(const Smp::Publication::IType* type);
    template <typename T>
    static Json::Object parseKind(const T& kind);
    static Json::Object parseField(const Smp::IField* field);
    static Json::Object parseEP(const Smp::IEntryPoint* ep);
    static Json::Array parseFields(const Smp::IComponent* Component);
    static Json::Array parseEPs(const Smp::IComponent* Component);
    static Json::Object parseComponent(const Smp::IComponent* component, bool recursive);
    static Json::Array parseContainer(const Smp::IContainer* container, bool recursive);
    void getSimulator(const HttpReq* req, HttpResp* resp) const;
    void getState(const HttpReq* req, HttpResp* resp) const;
    void defaultGetHandler(const HttpReq* req, HttpResp* resp) const;

    HttpServer _server;
    const Smp::ISimulator* _sim;
    Smp::Services::IResolver* _rslv;
    Smp::Services::ITimeKeeper* _tk;
};

} /* namespace rest */
} /* namespace simphonie */
#endif /* __simphonie_rest_RestService_HPP__ */
