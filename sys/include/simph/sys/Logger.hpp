/*
 * @file Logger.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_sys_Logger_HPP__
#define __simph_sys_Logger_HPP__
#include <mutex>
#include <iostream>
#include <sstream>

namespace simph {
	namespace sys {

/**
 *
 */
class Logger {
public:
    /**
     * Default constructor.
     */
    Logger();
    /**
     * Destructor.
     */
    virtual ~Logger();

    virtual void log(std::string level, std::string msg);
private:
    std::ostream& _out;
    std::mutex _mutex;
};

#if _abs_trace_debug
#define TRACE(msg) {\
    std::ostringstream _simph_sys_logger_stream_;\
    _simph_sys_logger_stream_ << __FILE__ << ":" << __LINE__ << "\t" << msg;\
    simph::sys::_logger->log("TRACE",_simph_sys_logger_stream_.str());\
}
#define LOGD(msg) {\
    std::ostringstream _simph_sys_logger_stream_;\
    _simph_sys_logger_stream_ << msg;\
    simph::sys::_logger->log("DEBUG",_simph_sys_logger_stream_.str());\
}
#else
#define TRACE(msg)
#define LOGD(msg)
#endif
#define LOGI(msg) {\
    std::ostringstream _simph_sys_logger_stream_;\
    _simph_sys_logger_stream_ << msg;\
    simph::sys::_logger->log("INFO",_simph_sys_logger_stream_.str());\
}
#define LOGW(msg) {\
    std::ostringstream _simph_sys_logger_stream_;\
    _simph_sys_logger_stream_ << msg;\
    simph::sys::_logger->log("WARNING",_simph_sys_logger_stream_.str());\
}
#define LOGE(msg) {\
    std::ostringstream _simph_sys_logger_stream_;\
    _simph_sys_logger_stream_ << msg;\
    simph::sys::_logger->log("ERROR",_simph_sys_logger_stream_.str());\
}

extern simph::sys::Logger* _logger;

}} // namespace simph::sys
#endif // __simph_sys_Logger_HPP__
