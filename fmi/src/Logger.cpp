/*
 * @file Logger.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/fmi/Logger.hpp"

namespace simphonie {
namespace fmi {
// --------------------------------------------------------------------
// ..........................................................
Logger::Logger(fmi2String instanceName, fmi2ComponentEnvironment env,
               fmi2CallbackLogger logger, Smp::IObject* parent):
            Parent("FMILogger","FMI guest bridge to host logger", parent),
            _instanceName(instanceName),
            _env(env),
            _logger(logger) {
}
// ..........................................................
Logger::~Logger() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::Services::LogMessageKind Logger::QueryLogMessageKind(Smp::String8 messageKindName) {
    if (strcmp(Smp::Services::ILogger::LMK_InformationName, messageKindName)==0) {
        return Smp::Services::ILogger::LMK_Information;
    }
    if (strcmp(Smp::Services::ILogger::LMK_DebugName, messageKindName)==0) {
        return Smp::Services::ILogger::LMK_Debug;
    }
    if (strcmp(Smp::Services::ILogger::LMK_ErrorName, messageKindName)==0) {
        return Smp::Services::ILogger::LMK_Error;
    }
    if (strcmp(Smp::Services::ILogger::LMK_WarningName, messageKindName)==0) {
        return Smp::Services::ILogger::LMK_Warning;
    }
    if (strcmp(Smp::Services::ILogger::LMK_EventName, messageKindName)==0) {
        return Smp::Services::ILogger::LMK_Event;
    }
    return Smp::Services::ILogger::LMK_Information;
}
// ..........................................................
static fmi2Status _fmiLogLevels[]={fmi2OK, fmi2Discard, fmi2Error, fmi2Warning, fmi2OK};
// ..........................................................
void Logger::Log( const Smp::IObject* sender,
                Smp::String8 message,
                Smp::Services::LogMessageKind kind) {
    _logger(_env, _instanceName, _fmiLogLevels[kind], sender->GetName(), message);
    
}

}} // namespace simphonie::fmi
