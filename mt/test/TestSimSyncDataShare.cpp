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
    class FakeSimulator : public virtual Smp::ISimulator {
    public:
        FakeSimulator() : _treg(new simphonie::kern::TypeRegistry("TypeRegistry", "", nullptr)) {}
        virtual ~FakeSimulator() {
            delete _treg;
        }
        Smp::Publication::ITypeRegistry* GetTypeRegistry() const override {
            return _treg;
        }

        void Initialise() override {}
        void Publish() override {}
        void Configure() override {}
        void Connect() override {}
        void Run() override {}
        void Hold(Smp::Bool immediate) override {}
        void Store(Smp::String8 filename) override {}
        void Restore(Smp::String8 filename) override {}
        void Reconnect(Smp::IComponent* root) override {}
        void Exit() override {}
        void Abort() override {}
        Smp::SimulatorStateKind GetState() const override {
            return Smp::SimulatorStateKind::SSK_Standby;
        }
        void AddInitEntryPoint(Smp::IEntryPoint* entryPoint) override {}
        void AddModel(Smp::IModel* model) override {}
        void AddService(Smp::IService* service) override {}
        Smp::IService* GetService(Smp::String8 name) const override {
            return nullptr;
        }
        Smp::Services::ILogger* GetLogger() const override {
            return nullptr;
        }
        Smp::Services::ITimeKeeper* GetTimeKeeper() const override {
            return nullptr;
        }
        Smp::Services::IScheduler* GetScheduler() const override {
            return nullptr;
        }
        Smp::Services::IEventManager* GetEventManager() const override {
            return nullptr;
        }
        Smp::Services::IResolver* GetResolver() const override {
            return nullptr;
        }
        Smp::Services::ILinkRegistry* GetLinkRegistry() const override {
            return nullptr;
        }
        void RegisterFactory(Smp::IFactory* componentFactory) override {}
        Smp::IComponent* CreateInstance(Smp::Uuid uuid, Smp::String8 name, Smp::String8 description,
                                               Smp::IComposite* parent) override {
            return nullptr;
        }
        Smp::IFactory* GetFactory(Smp::Uuid uuid) const override {
            return nullptr;
        }
        const Smp::FactoryCollection* GetFactories() const override {
            return nullptr;
        }
        void LoadLibrary(Smp::String8 libraryPath, Smp::LibraryLoadingFlag flag) override {}
        const Smp::ContainerCollection* GetContainers() const override {
            return nullptr;
        }
        Smp::IContainer* GetContainer(Smp::String8 name) const override {
            return nullptr;
        }
        Smp::String8 GetName() const {
            return nullptr;
        }
        Smp::String8 GetDescription() const override {
            return nullptr;
        }
        Smp::IObject* GetParent() const override {
            return nullptr;
        }
        Smp::IObject* GetChild(Smp::String8 name) const override {
            return nullptr;
        }

    private:
        Smp::Publication::ITypeRegistry* _treg;
    };

Smp::Int64 _val1, _val2;
Smp::Float32 _val3, _val4;
Smp::IComponent* _comp;
SimSyncDataShare* _dataShare;
Smp::IPublication* _receiver;
FakeSimulator* _sim;

public:
void setUp() {
    _comp = new simdeck::Component("comp", "");
    _dataShare = new SimSyncDataShare();
    _sim = new FakeSimulator();
    _receiver = new simphonie::kern::Publication(_comp, _sim);
    _receiver->PublishField("field1", "", &_val1, Smp::ViewKind::VK_All, false, true, false);
    _receiver->PublishField("field2", "", &_val2, Smp::ViewKind::VK_All, false, false, true);
    _receiver->PublishField("field3", "", &_val3, Smp::ViewKind::VK_All, false, true, false);
    _receiver->PublishField("field4", "", &_val4, Smp::ViewKind::VK_All, false, false, true);
}

void tearDown() {
    delete _comp;
    delete _dataShare;
    delete _sim;
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
