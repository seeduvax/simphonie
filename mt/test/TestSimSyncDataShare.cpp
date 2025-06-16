/*
 * @file TestSimSyncDataShare.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "Smp/Float32.h"
#include "Smp/IField.h"
#include "Smp/Int64.h"
#include "abs/test.h"
#include "simdeck/Component.hpp"
#include "simdeck/Field.hpp"
#include "simphonie/kern/Publication.hpp"
#include "simphonie/kern/TypeRegistry.hpp"
#include "simphonie/mt/SimSyncDataShare.hpp"

namespace test {
using namespace simphonie::mt;

ABS_TEST_SUITE_BEGIN(SimSyncDataShare)

private:
Smp::Int64 _val1, _val2;
Smp::Float32 _val3, _val4;
Smp::IComponent* _comp;
SimSyncDataShare* _dataShare;
Smp::Publication::ITypeRegistry* _typeRegistry;
Smp::IPublication* _receiver;

public:
void setUp() {
    _comp = new simdeck::Component("comp", "");
    _dataShare = new SimSyncDataShare();
    _typeRegistry = new simphonie::kern::TypeRegistry("typeRegistry", "", nullptr);
    _receiver = new simphonie::kern::Publication(_comp, _typeRegistry);
    _receiver->PublishField("field1", "", &_val1, Smp::ViewKind::VK_All, false, true, false);
    _receiver->PublishField("field2", "", &_val2, Smp::ViewKind::VK_All, false, false, true);
    _receiver->PublishField("field3", "", &_val3, Smp::ViewKind::VK_All, false, true, false);
    _receiver->PublishField("field4", "", &_val4, Smp::ViewKind::VK_All, false, false, true);
}

void tearDown() {
    delete _comp;
    delete _dataShare;
    delete _typeRegistry;
    delete _receiver;
}

ABS_TEST_CASE_BEGIN(UsualCase) {
    _val1 = 18L;
    _val2 = 22L;
    _val3 = 5.2f;
    _val4 = 8.5f;

    _dataShare->publish(_comp, _receiver);
    const auto inputs = dynamic_cast<Smp::IOutputField*>(_receiver->GetField("inputs"));
    const auto outputs = dynamic_cast<Smp::IOutputField*>(_receiver->GetField("outputs"));
    outputs->Connect(_receiver->GetField("field2"));
    outputs->Connect(_receiver->GetField("field4"));
    inputs->Connect(_receiver->GetField("field1"));
    inputs->Connect(_receiver->GetField("field3"));

    const auto data = _dataShare->retrieveData();
    _dataShare->loadData(data);

    CPPUNIT_ASSERT_EQUAL(22L, _val1);
    CPPUNIT_ASSERT_EQUAL(22L, _val2);
    CPPUNIT_ASSERT_EQUAL(8.5f, _val3);
    CPPUNIT_ASSERT_EQUAL(8.5f, _val4);
}
ABS_TEST_CASE_END

ABS_TEST_SUITE_END
} /* namespace test */
