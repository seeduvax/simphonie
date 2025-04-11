/*
 * @file Logger.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/sys/Logger.hpp"
#include <ctime>
#include <iomanip>

namespace simphonie {
namespace sys {
simphonie::sys::Logger _loggerInternal;
simphonie::sys::Logger& _logger = _loggerInternal;
// --------------------------------------------------------------------
// ..........................................................
Logger::Logger(std::ostream& out) : _out(out) {}
// ..........................................................
Logger::~Logger() {}
// ..........................................................
void Logger::log(std::string level, std::string msg) {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    std::lock_guard<std::mutex> sync(_mutex);
    _out << t.tv_sec << ".";
    _out << std::setfill('0') << std::setw(9) << t.tv_nsec << "\t" << level << "\t" << msg << std::endl;
}
}  // namespace sys
}  // namespace simph
