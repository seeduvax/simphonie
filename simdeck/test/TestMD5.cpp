/*
 * @file TestMD5.cpp
 *
 * Copyright 2024 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "abs/test.h"
#include "simdeck/MD5.hpp"

namespace test {
using namespace simdeck;

// ----------------------------------------------------------
// test suite implementation
ABS_TEST_SUITE_BEGIN( MD5 )
// uncomment and cmplete next line for test suite description
// ABS_TEST_DESCR(test description)

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

    ABS_TEST_CASE_BEGIN(Digest)
        ABS_TEST_DESCR(Check MD5 computation using some reference digest found in wikipedia MD5 article.)
        // 128bits digest storage
#define CHECK_DIGEST(ed1,ed2,ed3,ed4,ed5,ed6,ed7,ed8,ed9,ed10,ed11,ed12,ed13,ed14,ed15,ed16) \
   uint8_t expect[16]={ed1,ed2,ed3,ed4,ed5,ed6,ed7,ed8,ed9,ed10,ed11,ed12,ed13,ed14,ed15,ed16};\
   auto digest=md5.getDigest();\
   for (int i=0;i<16;i++) {\
      CPPUNIT_ASSERT_EQUAL(expect[i],digest[i]);\
   }
        {
            MD5 md5;
            std::string str="The quick brown fox jumps over the lazy dog";
            md5.update((const uint8_t*)str.c_str(),str.size());
            md5.finalize();
            CHECK_DIGEST( 0x9e, 0x10, 0x7d, 0x9d, 0x37, 0x2b, 0xb6, 0x82,
                          0x6b, 0xd8, 0x1d, 0x35, 0x42, 0xa4, 0x19, 0xd6 )
        }
        {
            MD5 md5;
            std::string str="The quick brown fox jumps over the lazy dog.";
            md5.update((const uint8_t*)str.c_str(),str.size());
            md5.finalize();
            CHECK_DIGEST( 0xe4, 0xd9, 0x09, 0xc2, 0x90, 0xd0, 0xfb, 0x1c,
                          0xa0, 0x68, 0xff, 0xad, 0xdf, 0x22, 0xcb, 0xd0 )
        }
        {
            MD5 md5;
            std::string str="";
            md5.update((const uint8_t*)str.c_str(),str.size());
            md5.finalize();
            CHECK_DIGEST( 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
                          0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e )
        }
        {
            uint8_t buf[2560];
            std::ifstream f("test/random.bin", std::ios::binary);
            f.read(reinterpret_cast<char*>(buf),sizeof(buf));
            MD5 md5;
            md5.update(buf,sizeof(buf));
            md5.finalize();
            CHECK_DIGEST( 0xf0, 0x48, 0x78, 0x00, 0xdd, 0x86, 0x50, 0x33,
                          0xd7, 0x7a, 0xc3, 0xa7, 0x0c, 0x16, 0x5e, 0xac )
        }
    ABS_TEST_CASE_END
ABS_TEST_SUITE_END
} // namespace test
