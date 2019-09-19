/*
 * @file RttiUtil.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_sys_RttiUtil_HPP__
#define __simph_sys_RttiUtil_HPP__
#include <string>
#include <cxxabi.h>

namespace simph {
	namespace sys {

/**
 *
 */
class RttiUtil {
public:
    /**
     * Default constructor.
     */
    RttiUtil();
    /**
     * Destructor.
     */
    virtual ~RttiUtil();

    template <typename T>
    static inline std::string getTypeName(T* ptr) {
        return demangle(typeid(*ptr).name());
    }
    static inline std::string demangle(std::string name) {
        int status=-1;
        char* dn=abi::__cxa_demangle(name.c_str(),NULL,NULL,&status);
        if (status==0) {
            name=dn;
            std::free(dn);
        }
        return name;
    }

private:

};

}} // namespace simph::sys
#endif // __simph_sys_RttiUtil_HPP__
