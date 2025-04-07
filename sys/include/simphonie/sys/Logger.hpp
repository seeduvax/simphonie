/*
 * @file Logger.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_sys_Logger_HPP__
#define __simphonie_sys_Logger_HPP__
#include <iostream>
#include <sstream>
#include <mutex>

namespace simphonie {
namespace sys {

/**
 *
 */
class Logger {
public:
    /**
     * Default constructor.
     */
    Logger(std::ostream& out = std::cout);
    /**
     * Destructor.
     */
    virtual ~Logger();

    virtual void log(std::string level, std::string msg);

    // TODO add enum for log level
private:
    std::ostream& _out;
    std::mutex _mutex;
};

#if _abs_trace_debug
#define TRACE(msg)                                                               \
    {                                                                            \
        std::ostringstream _simphonie_sys_logger_stream_;                            \
        _simphonie_sys_logger_stream_ << __FILE__ << ":" << __LINE__ << "\t" << msg; \
        simphonie::sys::_logger.log("TRACE", _simphonie_sys_logger_stream_.str());      \
    }
#define LTRACE(expr) TRACE(#expr << " = " << expr)
#define LOGD(msg)                                                           \
    {                                                                       \
        std::ostringstream _simphonie_sys_logger_stream_;                       \
        _simphonie_sys_logger_stream_ << msg;                                   \
        simphonie::sys::_logger.log("DEBUG", _simphonie_sys_logger_stream_.str()); \
    }
#else
#define TRACE(msg)
#define LOGD(msg)
#endif
#define LOGI(msg)                                                          \
    {                                                                      \
        std::ostringstream _simphonie_sys_logger_stream_;                      \
        _simphonie_sys_logger_stream_ << msg;                                  \
        simphonie::sys::_logger.log("INFO", _simphonie_sys_logger_stream_.str()); \
    }
#define LOGW(msg)                                                             \
    {                                                                         \
        std::ostringstream _simphonie_sys_logger_stream_;                         \
        _simphonie_sys_logger_stream_ << msg;                                     \
        simphonie::sys::_logger.log("WARNING", _simphonie_sys_logger_stream_.str()); \
    }
#define LOGE(msg)                                                           \
    {                                                                       \
        std::ostringstream _simphonie_sys_logger_stream_;                       \
        _simphonie_sys_logger_stream_ << msg;                                   \
        simphonie::sys::_logger.log("ERROR", _simphonie_sys_logger_stream_.str()); \
    }

extern simphonie::sys::Logger& _logger;

}  // namespace sys
}  // namespace simph
#endif  // __simphonie_sys_Logger_HPP__
