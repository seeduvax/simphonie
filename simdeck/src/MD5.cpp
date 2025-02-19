/*
 * @file MD5.cpp
 *
 * Copyright 2024 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * Inspired by https://github.com/Zunawe/md5-c
 *
 * $Id$
 * $Date$
 */
#include "simdeck/MD5.hpp"
#include "simdeck/endian.h"
#include <iostream>

#define F(X, Y, Z) ((X & Y) | (~X & Z))
#define G(X, Y, Z) ((X & Z) | (Y & ~Z))
#define H(X, Y, Z) (X ^ Y ^ Z)
#define I(X, Y, Z) (Y ^ (X | ~Z))

namespace simdeck {

static uint32_t S[64]=
        { 7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
          5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
          4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23, 
          6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21 };
static uint32_t K[64]=
        { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
          0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
          0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
          0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
          0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
          0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
          0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
          0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
          0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
          0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
          0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
          0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
          0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
          0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
          0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
          0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };
static uint8_t PADDING[] = {
          0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// --------------------------------------------------------------------
// ..........................................................
MD5::MD5() {
    _size=0;

    _buf[0]=0x67452301;
    _buf[1]=0xefcdab89;
    _buf[2]=0x98badcfe;
    _buf[3]=0x10325476;
}
// ..........................................................
MD5::~MD5() {
}

#if COMPILER_HOST_BYTE_ORDER==COMPILER_HOST_LITTLE_ENDIAN
#define I32_MSB  3
#define I32_MMSB  2
#define I32_MLSB  1
#define I32_LSB  0
#else
#define I32_MSB  0
#define I32_MMSB  1
#define I32_MLSB  2
#define I32_LSB  3
#endif
// --------------------------------------------------------------------
// ..........................................................
void MD5::update(const uint8_t* inBuf, size_t inputLen) {
    uint32_t input[16];
    unsigned int offset = _size % 64;
    _size += inputLen;

    for(unsigned int i = 0; i < inputLen; ++i){
        _input[offset] = inBuf[i];
        offset++;

        if(offset % 64 == 0){
            for(unsigned int j = 0; j < 16; ++j){
                input[j] = (_input[(j * 4) + I32_MSB]) << 24 |
                           (_input[(j * 4) + I32_MMSB]) << 16 |
                           (_input[(j * 4) + I32_MLSB]) <<  8 |
                           (_input[(j * 4) + I32_LSB]);
            }
            step(input);
            offset = 0;
        }
    }
}
// ..........................................................
void MD5::finalize() {
    uint32_t input[16];
    unsigned int offset = _size % 64;
    unsigned int paddingLen = offset < 56 ? 56 - offset : (56 + 64) - offset;

    update(PADDING, paddingLen);
    _size -= (uint64_t)paddingLen;

    /* Do a final update (internal to this function)
     * Last two 32-bit words are the two halves of the size (converted from bytes to bits)
     */ 
    for(unsigned int j = 0; j < 14; ++j){
        input[j] = ntoh32(*reinterpret_cast<uint32_t*>(&_input[j * 4]));
        input[j] = _input[(j * 4) + I32_MSB] << 24 |
                   _input[(j * 4) + I32_MMSB] << 16 |
                   _input[(j * 4) + I32_MLSB] <<  8 |
                   _input[(j * 4) + I32_LSB];
    }
    input[14] = _size * 8;
    input[15] = (_size * 8) >> 32;

    step(input);

    /* convert result from little-endian)
     */ 
    for(unsigned int i = 0; i < 4; ++i){
        _digest[(i * 4) + I32_LSB] = (_buf[i] & 0x000000FF);
        _digest[(i * 4) + I32_MLSB] = (_buf[i] & 0x0000FF00) >>  8;
        _digest[(i * 4) + I32_MMSB] = (_buf[i] & 0x00FF0000) >> 16;
        _digest[(i * 4) + I32_MSB] = (_buf[i] & 0xFF000000) >> 24;
    }
}
// ..........................................................
void MD5::step(uint32_t* input) {
    uint32_t AA = _buf[0];
    uint32_t BB = _buf[1];
    uint32_t CC = _buf[2];
    uint32_t DD = _buf[3];
    uint32_t E;
    unsigned int j;

    for(unsigned int i = 0; i < 64; ++i){
        switch(i / 16){
            case 0:
                E = F(BB, CC, DD);
                j = i;
                break;
            case 1:
                E = G(BB, CC, DD);
                j = ((i * 5) + 1) % 16;
                break;
            case 2:
                E = H(BB, CC, DD);
                j = ((i * 3) + 5) % 16;
                break;
            default:
                E = I(BB, CC, DD);
                j = (i * 7) % 16;
                break;
        }

        uint32_t temp = DD;
        DD = CC;
        CC = BB;
        BB = BB + rotateLeft(AA + E + K[i] + input[j], S[i]);
        AA = temp;
    }

    _buf[0] += AA;
    _buf[1] += BB;
    _buf[2] += CC;
    _buf[3] += DD;
}

// ..........................................................
uint32_t MD5::rotateLeft(uint32_t x, uint32_t n) {
    return (x << n) | (x >> (32 - n));
}


} // namespace simdeck
