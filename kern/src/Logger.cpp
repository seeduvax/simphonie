/*
 * @file Logger.cpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/Logger.hpp"
#include <cstring>
#include "Smp/ISimulator.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simphonie/kern/ILoggerBackend.hpp"
#include "simphonie/kern/LoggerOStream.hpp"
#include "simphonie/kern/Publication.hpp"

namespace simphonie {
namespace kern {

const Smp::String8 Logger::_LMK_NamesTable[] = {
    Smp::Services::ILogger::LMK_InformationName, Smp::Services::ILogger::LMK_EventName,
    Smp::Services::ILogger::LMK_WarningName, Smp::Services::ILogger::LMK_ErrorName,
    Smp::Services::ILogger::LMK_DebugName};

Logger::Logger(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent) : Component(name, descr, parent) {
    addContainer("Main Logger Container");
    GetContainer("Main Logger Container")
        ->AddComponent(new LoggerOstream("LoggerOstream", "Logger to stdout/stderr/stdlog"));
}

Smp::Services::LogMessageKind Logger::QueryLogMessageKind(Smp::String8 messageKindName) {
    for (int i = 0; i <= Smp::Services::ILogger::LMK_Debug; i++) {
        if (strcmp(messageKindName, _LMK_NamesTable[i]) == 0) {
            return i;
        }
    }
    return Smp::Services::ILogger::LMK_Debug;
}

void Logger::Log(const Smp::IObject* sender, Smp::String8 message, Smp::Services::LogMessageKind kind) {
    LoggerEvent event;
    ILoggerBackend* logger;

    const Smp::Services::ITimeKeeper* time = getSimulator()->GetTimeKeeper();
    event.setZuluTime(time->GetZuluTime());
    event.setSimulationTime(time->GetSimulationTime());
    event.setSenderName(sender->GetName());
    event.setMessage(message);
    event.setKind(kind);
    event.setThreadId(std::this_thread::get_id());
    event.build();

    const std::lock_guard<std::mutex> lock(_mutex);

    for (auto container : *GetContainers()) {
        for (auto component : *(container->GetComponents())) {
            logger = dynamic_cast<ILoggerBackend*>(component);
            if (logger) {
                logger->log(event);
            }
        }
    }
}

} /* namespace kern */
}  // namespace simphonie
