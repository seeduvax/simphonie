/*
 * @file LoggerEvent.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_LoggerEvent_HPP__
#define __simphonie_kern_LoggerEvent_HPP__

#include <thread>
#include "Smp/DateTime.h"
#include "Smp/Services/ILogger.h"
#include "Smp/String8.h"

namespace simphonie {
namespace kern {

/**
 *
 */
class LoggerEvent {
public:
    /**
     * Default constructor.
     */
    LoggerEvent();
    /**
     * Destructor.
     */
    virtual ~LoggerEvent();

    Smp::DateTime _simulationTime;
    Smp::DateTime _zuluTime;
    Smp::String8 _senderName;
    std::string _msg;
    Smp::Services::LogMessageKind _kind;
    std::thread::id _threadId;
};

}  // namespace kern
}  // namespace simphonie
#endif  // __simphonie_kern_LoggerEvent_HPP__
