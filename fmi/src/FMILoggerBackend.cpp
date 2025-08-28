/*
 * @file FMILoggerBackend.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/fmi/FMILoggerBackend.hpp"

namespace simphonie {
namespace fmi {
// --------------------------------------------------------------------
// ..........................................................
FMILoggerBackend::FMILoggerBackend(cppfmu::Logger logger, Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simdeck::Component(name, descr, parent), _logger(logger) {}

void FMILoggerBackend::log(const simphonie::kern::LoggerEvent& event) {
    /* Using the cathegory field as the instance name, this may not be allowed */
    switch (event.getKind()) {
        case Smp::Services::ILogger::LMK_Error:
            _logger.Log(cppfmu::FMIStatus::fmi2Error, event.getSender()->GetName(), event.getString());
            break;
        case Smp::Services::ILogger::LMK_Warning:
            _logger.Log(cppfmu::FMIStatus::fmi2Warning, event.getSender()->GetName(), event.getString());
            break;
        case Smp::Services::ILogger::LMK_Event:
        case Smp::Services::ILogger::LMK_Information:
            _logger.Log(cppfmu::FMIStatus::fmi2OK, event.getSender()->GetName(), event.getString());
            break;
        case Smp::Services::ILogger::LMK_Debug:
            _logger.DebugLog(cppfmu::FMIStatus::fmi2OK, event.getSender()->GetName(), event.getString());
            break;
    }
}

}  // namespace fmi
}  // namespace simphonie
