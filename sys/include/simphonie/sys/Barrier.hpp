/*
 * @file Barrier.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_sys_Barrier_HPP__
#define __simphonie_sys_Barrier_HPP__

#include <mutex>
#include <condition_variable>

namespace simphonie {
namespace sys {

class Barrier {
public:
    Barrier(size_t threshold = 0);
    ~Barrier() = default;

    void setThreshold(size_t threshold);
    bool wait();
    void cancel();
    void reset();
    bool isTriggered() { return _triggered; };

private:
    size_t _thresh;
    size_t _generation, _waiting, _released;
    bool _triggered, _canceled;
    std::mutex _mutex;
    std::condition_variable _cond;

};

}  /* namespace sys */
}  /* namespace simphonie */
#endif /* __simphonie_sys_Barrier_HPP__ */
