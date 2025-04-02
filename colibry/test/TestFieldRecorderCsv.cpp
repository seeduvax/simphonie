/*
 * @file TestFieldRecorderCsv.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "abs/test.h"
#include "simphonie/colibry/FieldRecorderCsv.hpp"

#include "simdeck/SimpleField.hpp"

namespace test {
using namespace simphonie::colibry;

// ----------------------------------------------------------
// test suite implementation
ABS_TEST_SUITE_BEGIN( FieldRecorderCsv )
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
        FieldRecorderCsv recorder("testfrec","",nullptr);
        recorder.Publish(nullptr);
        recorder.Configure(nullptr);
        recorder.Connect((Smp::ISimulator*)nullptr);

        Smp::Float64 dblV=42.0;
        Smp::Int32 intV=42;
        auto dblF=simdeck::SimpleField::Create("dbl","",Smp::ViewKind::VK_None, 
                                          &dblV, false, false, false, nullptr,
                                          Smp::Uuids::Uuid_Float64);
        auto intF=simdeck::SimpleField::Create("int32","",Smp::ViewKind::VK_None, 
                                          &intV, false, false, false, nullptr,
                                          Smp::Uuids::Uuid_Int32);
        recorder.Connect(dblF);
        recorder.Connect(intF);
    
        recorder.step();
        dblV-=1.1;
        intV++;
        recorder.step();

        // Do not disconect really because since the test does not used a full
        // featured ISimulator instance on Connect, actual component
        // implementation is failing when cleaning linkks of LinkRegistry.
        // recorder.Disconnect();
    }
    ABS_TEST_CASE_END
ABS_TEST_SUITE_END
} // namespace test
