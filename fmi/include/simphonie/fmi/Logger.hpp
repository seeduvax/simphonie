/*
 * @file Logger.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_fmi_Logger_HPP__
#define __simphonie_fmi_Logger_HPP__

#include "simdeck/Service.hpp"
#include "Smp/Services/ILogger.h"

namespace simphonie {
namespace fmi {

/**
 *
 */
class Logger: public simdeck::Service, virtual public Smp::IServices::Logger {
public:
    /**
     * Default constructor.
     */
    Logger(fmi2String instanceName, fmi2ComponentEnvironment env, 
                        fmi2CallbackLogger logger, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~Logger();

    // Smp::Services::ILogger implentation
    Smp::Services::LogMessageKind QueryLogMessageKind(
        Smp::String8 messageKindName) override;
    virtual void Log(
        const Smp::IObject* sender,
        Smp::String8 message,
        Smp::Services::LogMessageKind kind = 0) override;
private:
    fmi2String _instanceName;
    fmi2ComponentEnvironment _env;
    fmi2CallbackLogger _logger;
};

}} // namespace simphonie::fmi
#endif // __simphonie_fmi_Logger_HPP__
