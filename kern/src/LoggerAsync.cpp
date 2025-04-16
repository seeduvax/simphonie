/*
 * @file LoggerAsync.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/LoggerAsync.hpp"
#include <sstream>
#include "simdeck/StringField.hpp"
#include "simphonie/kern/Logger.hpp"

namespace simphonie {
namespace kern {

LoggerAsync::LoggerAsync(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent) : Component(name, descr, parent) {
    _bufferSize = 16;
}

void LoggerAsync::publish(Smp::IPublication* receiver) {
    receiver->PublishField("bufferSize", "Size of the events' buffer", &_bufferSize, Smp::ViewKind::VK_All, false, true,
                           false);
}

void LoggerAsync::log(const LoggerEvent& event) {
    if (_bufferSize > 0) {
        std::lock_guard<std::mutex> lock(_mutex);

        while (_buffer.size() > _bufferSize) {
            _buffer.pop();
        }
        _buffer.push(event);
    }
    _cv.notify_one();
}

const LoggerEvent& LoggerAsync::getEvent() {
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock, [&] { return !_buffer.empty(); });

    return _buffer.front();
}

void LoggerAsync::consumeEvent() {
    std::lock_guard<std::mutex> lock(_mutex);

    if (!_buffer.empty()) {
        _buffer.pop();
    }
}

} /* namespace kern */
} /* namespace simphonie */
