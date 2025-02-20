/*
 * @file Logger.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_Logger_HPP__
#define __simphonie_kern_Logger_HPP__
#include "Smp/Services/ILogger.h"
#include "simdeck/Component.hpp"
#include "simphonie/sys/Logger.hpp"

namespace simphonie {
namespace kern {
using namespace simdeck;

/**
 *
 */
class Logger : virtual public Component, virtual public Smp::Services::ILogger {
public:
    /**
     * Default constructor.
     */
    Logger(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~Logger();
    // Smp::Services::ILogger implementation
    Smp::Services::LogMessageKind QueryLogMessageKind(Smp::String8 messageKindName) override;
    void Log(const Smp::IObject* sender, Smp::String8 message, Smp::Services::LogMessageKind kind = 0) override;

private:
    simphonie::sys::Logger _logger;
};

}  // namespace kern
}  // namespace simph
#endif  // __simphonie_kern_Logger_HPP__
