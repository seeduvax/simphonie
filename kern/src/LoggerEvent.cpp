/*
 * @file LoggerEvent.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/LoggerEvent.hpp"
#include <iomanip>
#include <sstream>
#include <string>
#include "Smp/Services/ILogger.h"
#include "simphonie/kern/Logger.hpp"

namespace simphonie {
namespace kern {

Smp::String8 LoggerEvent::buildString(const Smp::IObject* sender, const std::string message,
                                      const Smp::Services::LogMessageKind kind, const Smp::DateTime zuluTime,
                                      const Smp::Duration simulationTime, const std::thread::id threadId) {
    std::ostringstream s, s2;
    char* cstr;
    s << "[" << zuluTime << " (" << simulationTime << ")] [" << threadId << "] ";
    s2 << "[" << Logger::_LMKMap.at(kind).name << "]";
    s << std::setw(13) << s2.str();
    s << " [" << sender->GetName() << "] " << message << std::endl;
    cstr = new char[s.str().length() + 1];
    std::strcpy(cstr, s.str().c_str());
    return cstr;
}

Smp::String8 LoggerEvent::buildString(const Smp::String8 string) {
    char* cstr = new char[strlen(string) + 1];
    strcpy(cstr, string);
    return cstr;
}

LoggerEvent::LoggerEvent(const Smp::DateTime zuluTime, const Smp::Duration simulationTime, const Smp::IObject* sender,
                         const std::string message, const Smp::Services::LogMessageKind kind)
    : _threadId(std::this_thread::get_id()),
      _sender(sender),
      _message(message),
      _kind(kind),
      _zuluTime(zuluTime),
      _simulationTime(simulationTime),
      _string(LoggerEvent::buildString(sender, message, kind, zuluTime, simulationTime, _threadId)) {}

LoggerEvent::LoggerEvent(const LoggerEvent& other)
    : _sender(other._sender),
      _message(other._message),
      _kind(other._kind),
      _zuluTime(other._zuluTime),
      _simulationTime(other._simulationTime),
      _threadId(other._threadId),
      _string(LoggerEvent::buildString(other._string)) {}

LoggerEvent::~LoggerEvent() {
    delete[] _string;
}

std::ostream& operator<<(std::ostream& os, const LoggerEvent& event) {
    return os << event.getString();
}

} /* namespace kern */
} /* namespace simphonie */
