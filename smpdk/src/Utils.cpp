/*
 * @file Utils.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/Utils.hpp"
#include <openssl/md5.h>
#include <stdio.h>
#include <cstring>

namespace simph {
namespace smpdk {
// ..........................................................
void Utils::CalcMD5(const void* pBuf, size_t bufSize, uint8_t dest[UTILS_MD5_DIGEST_LENGTH]) {
    MD5_CTX ctx;
    ::MD5_Init(&ctx);
    ::MD5_Update(&ctx, pBuf, bufSize);
    ::MD5_Final(dest, &ctx);
}
// ..........................................................
std::string Utils::Hex(uint8_t* pBuf, size_t bufSize) {
    std::string ret;
    ret.reserve(bufSize * 2);
    const int HEX_LEN = 3;
    char hex[HEX_LEN];
    uint8_t* pCharBuf = (uint8_t*)pBuf;
    ::memset(hex, 0, HEX_LEN);
    for (uint16_t i = 0; i < bufSize; i++) {
        ::sprintf(hex, "%02x", pCharBuf[i]);
        ret += hex;
    }
    return ret;
    // char hexStr[bufSize*2+1];
    // uint8_t* p = reinterpret_cast<uint8_t*>(hexStr);
    // for (uint16_t i = 0; i < bufSize; i++) {
    //    p += i*2;
    //    ::sprintf(reinterpret_cast<char*>(p), "%02x", pCharBuf[i]);
    //}
    // return std::string(hexStr,bufSize*2);
}
// ..........................................................
Smp::Uuid Utils::generateUuid(std::string fullName) {
    uint8_t md5[UTILS_MD5_DIGEST_LENGTH];
    Utils::CalcMD5(fullName.c_str(), fullName.size(), md5);
    return Smp::Uuid(Utils::Hex(md5, UTILS_MD5_DIGEST_LENGTH).c_str());
}
// ..........................................................

}  // namespace smpdk
}  // namespace simph
