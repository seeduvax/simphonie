/*
 * @file LoggerAsync.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_LoggerAsync_HPP__
#define __simphonie_kern_LoggerAsync_HPP__

#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <thread>
#include "Smp/IObject.h"
#include "Smp/IPublication.h"
#include "Smp/String8.h"
#include "simdeck/Component.hpp"
#include "simdeck/Composite.hpp"
#include "simphonie/kern/ILoggerBackend.hpp"
#include "simphonie/kern/LoggerEvent.hpp"

namespace simphonie {
namespace kern {
using namespace simdeck;

class LoggerAsync : public Component, virtual public ILoggerBackend, virtual public AComposite {
public:
    LoggerAsync(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);

    void log(const LoggerEvent& event) override;
    const LoggerEvent& getEvent();
    void consumeEvent();
    Smp::Int32 count();

protected:
    void publish(Smp::IPublication* receiver);

private:
    std::mutex _mutex;
    std::condition_variable _cv;
    std::queue<LoggerEvent> _buffer;
    Smp::Int32 _bufferSize;
};

} /* namespace kern */
} /* namespace simphonie */

#endif /* __simphonie_kern_LoggerAsync_HPP__ */
