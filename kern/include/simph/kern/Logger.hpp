/*
 * @file Logger.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Logger_HPP__
#define __simph_kern_Logger_HPP__
#include "Smp/Services/ILogger.h"
#include "simph/kern/Object.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
	namespace kern {

/**
 *
 */
class Logger: virtual public Object, virtual public Smp::Services::ILogger {
public:
    /**
     * Default constructor.
     */
    Logger();
    /**
     * Destructor.
     */
    virtual ~Logger();
    // Smp::Services::ILogger implementation
    Smp::Services::LogMessageKind QueryLogMessageKind(
                             Smp::String8 messageKindName);
    void Log( const Smp::IObject* sender,
                Smp::String8 message,
                Smp::Services::LogMessageKind kind = 0);
private:
    simph::sys::Logger _logger;

};

}} // namespace simph::kern
#endif // __simph_kern_Logger_HPP__
