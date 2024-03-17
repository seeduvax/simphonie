/*
 * @file Thread.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_sys_Thread_HPP__
#define __simph_sys_Thread_HPP__
#include <string>
#include <thread>
#include "simph/sys/Runnable.hpp"
namespace simph {
namespace sys {

/**
 *
 */
class Thread {
public:
    /**
     * Default constructor.
     */
    Thread(std::string name, Runnable* toRun);
    /**
     * Destructor.
     */
    virtual ~Thread();

    void start();

    inline void run() {
        _toRun->run();
    }

    inline void join() {
        if (_th->joinable()) {
            _th->join();
        }
    }

    inline std::string getName() {
        return _name;
    }

    inline bool isCurrentThread() {
        return std::this_thread::get_id() == _th->get_id();
    }

private:
    Runnable* _toRun;
    std::string _name;
    std::thread* _th;
};

}  // namespace sys
}  // namespace simph
#endif  // __simph_sys_Thread_HPP__
