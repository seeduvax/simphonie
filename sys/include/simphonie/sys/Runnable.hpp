/*
 * @file Runnable.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_sys_Runnable_HPP__
#define __simphonie_sys_Runnable_HPP__

namespace simphonie {
namespace sys {

/**
 *
 */
class Runnable {
public:
    /**
     * Default constructor.
     */
    Runnable();
    /**
     * Destructor.
     */
    virtual ~Runnable();

    virtual void run() = 0;

private:
};

}  // namespace sys
}  // namespace simph
#endif  // __simphonie_sys_Runnable_HPP__
