/*
 * @file TestFieldRecorderHDF5.cpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "abs/test.h"
#include "simphonie/colibry/FieldRecorderHDF5.hpp"
#include "FieldRecorderCommon.hpp"
 
namespace test {
using namespace simphonie::colibry;
 
// ----------------------------------------------------------
// test suite implementation
ABS_TEST_SUITE_BEGIN( FieldRecorderHDF5 )
// uncomment and complete next line for test suite description
// ABS_TEST_DESCR(test description)
 
private:
 
public:
    void setUp() {
    }
     
    void tearDown() {
    }

    ABS_TEST_CASE_BEGIN(RecordSimpleFields) {
        ABS_TEST_DESCR(Chack FieldRecorderCsv can record few simple fields )
        recordSimpleFieldsCase<FieldRecorderHDF5>(".h5");
        // TODO some asserts on the recorded file.
    }
    ABS_TEST_CASE_END
 
ABS_TEST_SUITE_END
} // namespace test
