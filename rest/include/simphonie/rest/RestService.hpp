/*
 * @file RestService.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_rest_RestService_HPP__
#define __simphonie_rest_RestService_HPP__

#include "Smp/ISimulator.h"
#include "simdeck/Service.hpp"
#include "wfrest/HttpServer.h"

namespace simphonie {
namespace rest {

/**
 *
 */
class RestService : public simdeck::Service {
    typedef simdeck::Service Parent;

public:
    /**
     * Default constructor.
     */
    RestService(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~RestService();

private:
    wfrest::HttpServer _server;
    Smp::ISimulator* _sim;

    void getSimulator(const wfrest::HttpReq* req, wfrest::HttpResp* resp);
};

}  // namespace rest
}  // namespace simphonie
#endif  // __simphonie_rest_RestService_HPP__
