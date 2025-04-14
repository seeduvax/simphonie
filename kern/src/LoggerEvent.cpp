/*
 * @file LoggerEvent.cpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/LoggerEvent.hpp"
#include <sstream>
#include "Smp/Services/ILogger.h"
#include "simphonie/kern/Logger.hpp"

namespace simphonie {
namespace kern {

LoggerEvent::LoggerEvent() {
    _senderName = "";
    _message = "";
    _kind = Smp::Services::ILogger::LMK_Debug;
    _zuluTime = 0;
    _simulationTime = 0;
    _string = "";
}

void LoggerEvent::setSenderName(Smp::String8 senderName) {
    _senderName = senderName;
}

Smp::String8 LoggerEvent::getSenderName() const {
    return _senderName;
}

void LoggerEvent::setMessage(Smp::String8 message) {
    _message = message;
}

Smp::String8 LoggerEvent::getMessage() const {
    return _message;
}

void LoggerEvent::setKind(Smp::Services::LogMessageKind kind) {
    _kind = kind;
}

Smp::Services::LogMessageKind LoggerEvent::getKind() const {
    return _kind;
}

void LoggerEvent::setZuluTime(Smp::DateTime zuluTime) {
    _zuluTime = zuluTime;
}

Smp::DateTime LoggerEvent::getZuluTime() const {
    return _zuluTime;
}

void LoggerEvent::setSimulationTime(Smp::Duration simulationTime) {
    _simulationTime = simulationTime;
}

Smp::Duration LoggerEvent::getSimulationTime() const {
    return _simulationTime;
}

void LoggerEvent::setThreadId(std::thread::id threadId) {
    _threadId = threadId;
}

std::thread::id LoggerEvent::getThreadId() const {
    return _threadId;
}

void LoggerEvent::build() {
    std::ostringstream s;
    s << _zuluTime << "\t" << _simulationTime << "\t" << Logger::_LMK_NamesTable[_kind] << "\t";
    s << _threadId << "\t" << _senderName << "\t" << _message << std::endl;
    _string = s.str();
}

std::string LoggerEvent::getString() const {
    return _string;
}

std::ostream& operator<<(std::ostream& os, const LoggerEvent& event) {
    return os << event.getString() << std::endl;
}

} /* namespace kern */
}  // namespace simphonie
