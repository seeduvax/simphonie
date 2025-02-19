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

#include <Smp/Uuid.h>
#include <stdint.h>
#include <cstddef>
#include <string>

#define UTILS_MD5_DIGEST_LENGTH 16

namespace simdeck {

struct Utils {
    /**
     * Generate an uuid for given model.
     * @param fullName the full name of the model (with factory prefix).
     */
    static Smp::Uuid generateUuid(std::string fullName);

    static void CalcMD5(const void* pBuf, size_t bufSize, uint8_t dest[UTILS_MD5_DIGEST_LENGTH]);

    static std::string Hex(uint8_t* value, size_t size);
};

}  // namespace simdeck
#endif  // __simdeck_Utils_HPP__
