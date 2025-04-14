/*
 * @file LoggerEvent.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_LoggerEvent_HPP__
#define __simphonie_kern_LoggerEvent_HPP__

#include <cstring>
#include <thread>
#include "Smp/DateTime.h"
#include "Smp/Duration.h"
#include "Smp/Services/LogMessageKind.h"
#include "Smp/String8.h"

namespace simphonie {
namespace kern {

class LoggerEvent {
public:
    LoggerEvent();

    void build();

    void setSenderName(Smp::String8 senderName);
    Smp::String8 getSenderName() const;
    void setMessage(Smp::String8 message);
    Smp::String8 getMessage() const;
    void setKind(Smp::Services::LogMessageKind kind);
    Smp::Services::LogMessageKind getKind() const;
    void setZuluTime(Smp::DateTime zuluTime);
    Smp::DateTime getZuluTime() const;
    void setSimulationTime(Smp::Duration simulationTime);
    Smp::Duration getSimulationTime() const;
    void setThreadId(std::thread::id threadId);
    std::thread::id getThreadId() const;

    std::string getString() const;

private:
    Smp::String8 _senderName;
    Smp::String8 _message;
    Smp::Services::LogMessageKind _kind;
    Smp::DateTime _zuluTime;
    Smp::Duration _simulationTime;
    std::thread::id _threadId;

    std::string _string;
};

std::ostream& operator<<(std::ostream& os, const LoggerEvent& event);

} /* namespace kern */
} /* namespace simphonie */

#endif /* __simphonie_kern_LoggerEvent_HPP__ */
