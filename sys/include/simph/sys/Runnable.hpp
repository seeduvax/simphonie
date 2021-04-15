/*
 * @file Runnable.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_sys_Runnable_HPP__
#define __simph_sys_Runnable_HPP__

namespace simph {
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
#endif  // __simph_sys_Runnable_HPP__
