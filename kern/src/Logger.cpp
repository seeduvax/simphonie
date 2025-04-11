/*
 * @file Logger.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/Logger.hpp"
#include <cstring>
#include "Smp/ISimulator.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simphonie/kern/Publication.hpp"

namespace simphonie {
namespace kern {

static Smp::String8 _LMK_NamesTable[] = {Smp::Services::ILogger::LMK_InformationName,
                                         Smp::Services::ILogger::LMK_EventName, Smp::Services::ILogger::LMK_WarningName,
                                         Smp::Services::ILogger::LMK_ErrorName, Smp::Services::ILogger::LMK_DebugName};

Logger::Logger(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent) : Container(name, descr, parent) {
    _simulator = dynamic_cast<Smp::ISimulator*>(parent); /* TODO assuming the simulator is the parent */
}

Smp::Services::LogMessageKind Logger::QueryLogMessageKind(Smp::String8 messageKindName) {
    for (int i = 0; i <= Smp::Services::ILogger::LMK_Debug; i++) {
        if (strcmp(messageKindName, _LMK_NamesTable[i]) == 0) {
            return i;
        }
    }
    return Smp::Services::ILogger::LMK_Debug;
}

std::string Logger::buildLogString(LoggerEvent event) {
    std::ostringstream s;
    s << event.zuluTime << "\t" << event.simulationTime << "\t" << _LMK_NamesTable[event.kind] << "\t";
    s << event.threadId << "\t" << event.senderName << "\t" << event.message << std::endl;
    return s.str();
}

void Logger::Log(const Smp::IObject* sender, Smp::String8 message, Smp::Services::LogMessageKind kind) {
    LoggerEvent event;

    const Smp::Services::ITimeKeeper* time = _simulator->GetTimeKeeper();
    event.zuluTime = time->GetZuluTime();
    event.simulationTime = time->GetSimulationTime();
    event.senderName = sender->GetName();
    event.message = message;
    event.kind = kind;
    event.threadId = std::this_thread::get_id();

    const std::lock_guard<std::mutex> lock(_mutex);

    if (_eventsQueueSize > 0) {
        while (_eventsBuffer.size() > _eventsQueueSize - 1)
            _eventsBuffer.pop();
        _eventsBuffer.push(event);
    }

    for (auto loggerName : _loggersName) {
        GetChild(loggerName);
    }
}

} /* namespace kern */
}  // namespace simphonie
