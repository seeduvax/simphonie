/*
 * @file DLib.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_sys_DLib_HPP__
#define __simph_sys_DLib_HPP__
#include <string>
#include "simph/sys/Logger.hpp"

namespace simph {
namespace sys {

/**
 *
 */
class DLib {
public:
    /**
     * Default constructor.
     */
    DLib(const char* libName);
    /**
     * Destructor.
     */
    virtual ~DLib();

    template <typename FT>
    FT getEntry(const char* symbol) {
        return reinterpret_cast<FT>(getEntryHandle(symbol));
    }
    inline std::string getName() {
        return _name;
    }

private:
    void* getEntryHandle(const char* symbol);
    std::string _name;
    void* _lib;
};

}  // namespace sys
}  // namespace simph
#endif  // __simph_sys_DLib_HPP__
