/*
 * @file Barrier.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/sys/Barrier.hpp"

namespace simphonie {
namespace sys {

Barrier::Barrier(size_t threshold) : _thresh(threshold), _generation(0),
    _waiting(0), _released(0), _triggered(false), _canceled(false) {}

void Barrier::setThreshold(size_t threshold) {
    std::lock_guard<std::mutex> lock(_mutex);
    _thresh = threshold;
    if (_waiting >= _thresh) {
        _waiting = 0;
        _triggered = true;
        _cond.notify_all();
    }
}

bool Barrier::wait() {
    std::unique_lock<std::mutex> lock(_mutex);

    const auto generation = _generation;
    _waiting++;
    if (_waiting >= _thresh) {
        _waiting = 0;
        _triggered = true;
        _generation++;
        _cond.notify_all();
    }

    _cond.wait(lock, [&]() { return _canceled || (_triggered && generation < _generation); });

    _released++;
    if (_released >= _thresh) {
        _released = 0;
        _triggered = false;
    }

    return !_canceled;
}

void Barrier::cancel() {
    std::lock_guard<std::mutex> lock(_mutex);
    _canceled = true;
    _cond.notify_all();
}

void Barrier::reset() {
    std::lock_guard<std::mutex> lock(_mutex);
    _canceled = false;
    _triggered = false;
    _waiting = 0;
    _released = 0;
    _generation = 0;
    _cond.notify_all();
}

}  /* namespace sys */
}  /* namespace simphonie */
