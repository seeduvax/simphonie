/*
 * @file LoggerOStream.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/LoggerOStream.hpp"

namespace simphonie {
namespace kern {

LoggerOStream::LoggerOStream(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Component(name, descr, parent) {}

void LoggerOStream::log(const LoggerEvent& event) {
    switch (event.getKind()) {
        case Smp::Services::ILogger::LMK_Error:
            std::cerr << event;
            break;
        case Smp::Services::ILogger::LMK_Debug:
            std::clog << event;
            break;
        default:
            std::cout << event;
            break;
    }
}

} /* namespace kern */
} /* namespace simphonie */
