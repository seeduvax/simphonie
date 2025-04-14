/*
 * @file LoggerOstream.cpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */

#include "simphonie/kern/LoggerOStream.hpp"
#include "simphonie/kern/Logger.hpp"

namespace simphonie {
namespace kern {

LoggerOstream::LoggerOstream(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Component(name, descr, parent) {}

void LoggerOstream::log(LoggerEvent event) {
    switch (event.getKind()) {
        case Smp::Services::ILogger::LMK_Error:
            std::cerr << event;
        case Smp::Services::ILogger::LMK_Debug:
            std::clog << event;
        default:
            std::cout << event;
    }
}

} /* namespace kern */
}  // namespace simphonie
