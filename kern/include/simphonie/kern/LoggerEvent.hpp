/*
 * @file LoggerEvent.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_LoggerEvent_HPP__
#define __simphonie_kern_LoggerEvent_HPP__

#include <string>
#include <thread>
#include "Smp/DateTime.h"
#include "Smp/Duration.h"
#include "Smp/IObject.h"
#include "Smp/Services/LogMessageKind.h"
#include "Smp/String8.h"

namespace simphonie {
namespace kern {

class LoggerEvent {
public:
    LoggerEvent(const Smp::DateTime zuluTime, const Smp::Duration simulationTime, const Smp::IObject* sender,
                const std::string message, const Smp::Services::LogMessageKind kind);
    ~LoggerEvent();
    LoggerEvent(const LoggerEvent& other);

    inline const Smp::IObject* getSender() const {
        return _sender;
    };
    inline std::string getMessage() const {
        return _message;
    };
    inline Smp::Services::LogMessageKind getKind() const {
        return _kind;
    };
    inline Smp::DateTime getZuluTime() const {
        return _zuluTime;
    };
    inline Smp::Duration getSimulationTime() const {
        return _simulationTime;
    };
    inline std::thread::id getThreadId() const {
        return _threadId;
    };
    inline Smp::String8 getString() const {
        return _string;
    };

private:
    const Smp::IObject* _sender;
    const std::string _message;
    const Smp::Services::LogMessageKind _kind;
    const Smp::DateTime _zuluTime;
    const Smp::Duration _simulationTime;
    const std::thread::id _threadId;
    const Smp::String8 _string;

    static Smp::String8 buildString(const Smp::IObject* sender, const std::string message,
                                    const Smp::Services::LogMessageKind kind, const Smp::DateTime zuluTime,
                                    const Smp::Duration simulationTime, const std::thread::id threadId);
    static Smp::String8 buildString(const Smp::String8 string);
};

std::ostream& operator<<(std::ostream& os, const LoggerEvent& event);

} /* namespace kern */
} /* namespace simphonie */

#endif /* __simphonie_kern_LoggerEvent_HPP__ */
