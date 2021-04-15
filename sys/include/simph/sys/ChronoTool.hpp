/*
 * @file ChronoTool.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_sys_ChronoTool_HPP__
#define __simph_sys_ChronoTool_HPP__

#include <chrono>
#include <iostream>
#include <vector>
#include "simph/sys/Runnable.hpp"

namespace simph {
namespace sys {

/**
 * Wrapper of std::chrono
 * it intends to provide basic services
 * to measure execution time
 *
 * TODO Reuse/Adapt this to generate schedule statistics
 */
struct ChronoTool {
    struct Record {
        std::chrono::steady_clock::time_point stopTime, startTime;

        inline void start() {
            startTime = std::chrono::steady_clock::now();
        }
        inline void stop() {
            stopTime = std::chrono::steady_clock::now();
        }

        template <typename ToTime = std::chrono::milliseconds>
        auto count() {
            auto duration = std::chrono::duration_cast<ToTime>(stopTime - startTime);
            return duration.count();
        }
    };

    static Record execution(Runnable& toRun) {
        ChronoTool::Record rec;

        rec.start();
        toRun.run();
        rec.stop();

        return rec;
    }

    struct Statistic {
        std::vector<Record> records;
        // TODO
    };
};

}  // namespace sys
}  // namespace simph
#endif  // __simph_sys_ChronoTool_HPP__
