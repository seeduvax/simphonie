/*
 * @file Utils.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_Utils_HPP__
#define __simdeck_Utils_HPP__

#include <cstdint>
#include <cstddef>
#include <string>
#include <cxxabi.h>
#include "Smp/Uuid.h"
#include "Smp/ISimulator.h"

#define UTILS_MD5_DIGEST_LENGTH 16

namespace simdeck {

struct Utils {
    /**
     * Generate an uuid for given model.
     * @param fullName the full name of the model (with factory prefix).
     */
    static Smp::Uuid GenerateUuid(const std::string& fullName);

    static void CalcMD5(const void* pBuf, size_t bufSize, uint8_t dest[UTILS_MD5_DIGEST_LENGTH]);

    static std::string Hex(uint8_t* value, size_t size);
    
    static std::string Demangle(std::string name) {
        int status = -1;
        char* dn = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status);
        if (status == 0) {
            name = dn;
            std::free(dn);
        }
        return name;
    }

    static std::string GetFullName(const Smp::IObject* o);
    static void Dump(const Smp::IObject* from, int level=0, const Smp::ISimulator* sim=nullptr);
};

}  // namespace simdeck
#endif  // __simdeck_Utils_HPP__
