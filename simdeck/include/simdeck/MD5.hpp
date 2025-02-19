/*
 * @file MD5.hpp
 *
 * Copyright 2024 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_MD5_HPP__
#define __simdeck_MD5_HPP__

#include <cstddef>
#include <stdint.h>

namespace simdeck {

/**
 *
 */
class MD5 {
public:
    /**
     * Default constructor.
     */
    MD5();
    /**
     * Destructor.
     */
    virtual ~MD5();

    void update(const uint8_t* inBuf, size_t inputLen);
    void finalize();
    inline const uint8_t* getDigest() const {
        return _digest;
    }
    inline size_t getDigestSize() const {
        return sizeof(_digest);
    }
private:
    void step(uint32_t* input);
    static uint32_t rotateLeft(uint32_t x, uint32_t n);

    uint64_t _size;
    uint32_t _buf[4];
    uint8_t _input[64];
    uint8_t _digest[16];
};

} // namespace simdeck
#endif // __simdeck_MD5_HPP__
