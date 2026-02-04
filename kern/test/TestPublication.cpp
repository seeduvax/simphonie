/*
 * @file TestPublication.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <string.h>

#include "Smp/ISimpleArrayField.h"
#include "Smp/ISimpleField.h"
#include "Smp/ISimulator.h"
#include "Smp/Publication/IStructureType.h"
#include "Smp/IStructureField.h"
#include "abs/test.h"
#include "simdeck/Component.hpp"
#include "simdeck/StringField.hpp"
#include "simdeck/StringType.hpp"
#include "simphonie/kern/Publication.hpp"
#include "simphonie/kern/TypeRegistry.hpp"
#include "simphonie/sys/Logger.hpp"

namespace test {
using namespace simphonie::kern;
using namespace simdeck;

// ----------------------------------------------------------
// test fixture implementation
ABS_TEST_SUITE_BEGIN(Publication)

private:
    class FakeSimulator : public virtual Smp::ISimulator {
    public:
        inline FakeSimulator() : _treg(new TypeRegistry("TypeRegistry", "", nullptr)) {}
        inline Smp::Publication::ITypeRegistry* GetTypeRegistry() const override {
            return _treg;
        }

        inline void Initialise() override {}
        inline void Publish() override {}
        inline void Configure() override {}
        inline void Connect() override {}
        inline void Run() override {}
        inline void Hold(Smp::Bool immediate) override {}
        inline void Store(Smp::String8 filename) override {}
        inline void Restore(Smp::String8 filename) override {}
        inline void Reconnect(Smp::IComponent* root) override {}
        inline void Exit() override {}
        inline void Abort() override {}
        inline Smp::SimulatorStateKind GetState() const override {
            return Smp::SimulatorStateKind::SSK_Standby;
        }
        inline void AddInitEntryPoint(Smp::IEntryPoint* entryPoint) override {}
        inline void AddModel(Smp::IModel* model) override {}
        inline void AddService(Smp::IService* service) override {}
        inline Smp::IService* GetService(Smp::String8 name) const override {
            return nullptr;
        }
        inline Smp::Services::ILogger* GetLogger() const override {
            return nullptr;
        }
        inline Smp::Services::ITimeKeeper* GetTimeKeeper() const override {
            return nullptr;
        }
        inline Smp::Services::IScheduler* GetScheduler() const override {
            return nullptr;
        }
        inline Smp::Services::IEventManager* GetEventManager() const override {
            return nullptr;
        }
        inline Smp::Services::IResolver* GetResolver() const override {
            return nullptr;
        }
        inline Smp::Services::ILinkRegistry* GetLinkRegistry() const override {
            return nullptr;
        }
        inline void RegisterFactory(Smp::IFactory* componentFactory) override {}
        inline Smp::IComponent* CreateInstance(Smp::Uuid uuid, Smp::String8 name, Smp::String8 description,
                                               Smp::IComposite* parent) override {
            return nullptr;
        }
        inline Smp::IFactory* GetFactory(Smp::Uuid uuid) const override {
            return nullptr;
        }
        inline const Smp::FactoryCollection* GetFactories() const override {
            return nullptr;
        }
#ifdef LoadLibrary
// Workaround collision between macro and method name on windows.
#undef LoadLibrary
#endif
        inline void LoadLibrary(Smp::String8 libraryPath, Smp::LibraryLoadingFlag flag) override {}
        inline const Smp::ContainerCollection* GetContainers() const override {
            return nullptr;
        }
        inline Smp::IContainer* GetContainer(Smp::String8 name) const override {
            return nullptr;
        }
        inline Smp::String8 GetName() const {
            return nullptr;
        }
        inline Smp::String8 GetDescription() const override {
            return nullptr;
        }
        inline Smp::IObject* GetParent() const override {
            return nullptr;
        }
        inline Smp::IObject* GetChild(Smp::String8 name) const override {
            return nullptr;
        }

    private:
        TypeRegistry* _treg;
    };

public:
    void setUp() {}

    void tearDown() {}

    ABS_TEST_CASE_BEGIN(PublishField) {
        FakeSimulator sim;
        std::unique_ptr<simdeck::Component> component(new simdeck::Component("testObj", "dummy object for testing", nullptr));
        Publication pub(component.get(), &sim);

        Smp::Char8 testChar = 'A';
        pub.PublishField("Char", "char8 test pub", &testChar);
        Smp::ISimpleField* f = dynamic_cast<Smp::ISimpleField*>(pub.GetField("Char"));
        CPPUNIT_ASSERT(f != nullptr);
        CPPUNIT_ASSERT_EQUAL(f, dynamic_cast<Smp::ISimpleField*>(component->GetField("Char")));
        CPPUNIT_ASSERT(strcmp(f->GetName(), "Char") == 0);
        CPPUNIT_ASSERT_EQUAL('A', (char)f->GetValue());

        Smp::Int32 testInt32 = -17042;
        pub.PublishField("int32", "int32 test pub", &testInt32);
        f = dynamic_cast<Smp::ISimpleField*>(pub.GetField("int32"));
        CPPUNIT_ASSERT(f != nullptr);
        CPPUNIT_ASSERT_EQUAL(f, dynamic_cast<Smp::ISimpleField*>(component->GetField("int32")));
        CPPUNIT_ASSERT(strcmp(f->GetName(), "int32") == 0);
        CPPUNIT_ASSERT_EQUAL(-17042, (int32_t)f->GetValue());

        Smp::Float64 testDouble = 42.042;
        pub.PublishField("Double", "float 64 test pub", &testDouble);
        f = dynamic_cast<Smp::ISimpleField*>(pub.GetField("Double"));
        CPPUNIT_ASSERT_EQUAL(f, dynamic_cast<Smp::ISimpleField*>(component->GetChild("Double")));
        CPPUNIT_ASSERT(f != nullptr);
        CPPUNIT_ASSERT(strcmp(f->GetName(), "Double") == 0);
        CPPUNIT_ASSERT_EQUAL(42.042, (double)f->GetValue());

        const Smp::FieldCollection* fc = pub.GetFields();
        CPPUNIT_ASSERT(fc != nullptr);
        CPPUNIT_ASSERT_EQUAL((size_t)3, fc->size());
        CPPUNIT_ASSERT(fc->at("Char") != nullptr);
        CPPUNIT_ASSERT(fc->at("int32") != nullptr);
        CPPUNIT_ASSERT(fc->at("Double") != nullptr);
        CPPUNIT_ASSERT(fc->at("double64") == nullptr);
        CPPUNIT_ASSERT_EQUAL(fc->at("Char"), pub.GetField("Char"));
        CPPUNIT_ASSERT_EQUAL(fc->at("int32"), pub.GetField("int32"));
        CPPUNIT_ASSERT_EQUAL(fc->at("Double"), pub.GetField("Double"));
    }
    ABS_TEST_CASE_END
    ABS_TEST_CASE_BEGIN(PublishArrayField) {
        FakeSimulator sim;
        std::unique_ptr<simdeck::Component> component(new simdeck::Component("testObj", "dummy object for testing", nullptr));
        Publication pub(component.get(), &sim);

        Smp::Int32 iArray[] = {12, 17, 42};
        pub.PublishArray("iArray", "int array test pub", 3, iArray, Smp::PrimitiveTypeKind::PTK_Int32);

        Smp::ISimpleArrayField* f = dynamic_cast<Smp::ISimpleArrayField*>(pub.GetField("iArray"));
        CPPUNIT_ASSERT(f != nullptr);
        CPPUNIT_ASSERT(strcmp(f->GetName(), "iArray") == 0);
        CPPUNIT_ASSERT_EQUAL(12, (int32_t)f->GetValue(0));
        CPPUNIT_ASSERT_EQUAL(17, (int32_t)f->GetValue(1));
        CPPUNIT_ASSERT_EQUAL(42, (int32_t)f->GetValue(2));
    }
    ABS_TEST_CASE_END

    ABS_TEST_CASE_BEGIN(PublishStringField) {
        FakeSimulator sim;
        std::unique_ptr<simdeck::Component> component(
            new simdeck::Component("testObj", "dummy object for testing", nullptr));
        Publication pub(component.get(), &sim);

        std::string stringField = "String Field";
        auto f = pub.PublishField("strField", "std::string test pub", &stringField, StringType::UuidString);
        CPPUNIT_ASSERT(f != nullptr);
        auto child = component->GetField("strField");
        CPPUNIT_ASSERT(f == child);
        auto sf = dynamic_cast<StringField*>(f);
        CPPUNIT_ASSERT(sf != nullptr);
        std::string retrieve = (const char*)sf->GetValue();
        CPPUNIT_ASSERT(retrieve == "String Field");
    }
    ABS_TEST_CASE_END

    ABS_TEST_CASE_BEGIN(PublishStructureField) {
        FakeSimulator sim;
        struct MyStruct {
            Smp::UInt16 word;
            Smp::Float64 number;
        };

        // register structure type
        Smp::Uuid uuid("ba580576-39c0-4ea5-922c-b584cc8358c9");
        auto tReg=sim.GetTypeRegistry();
        auto myStructType=tReg->AddStructureType(
                    "MyStructType",
                    "",
                    uuid);
        myStructType->AddField("word","",Smp::Uuids::Uuid_UInt16,offsetof(struct MyStruct, word));
        myStructType->AddField("number","",Smp::Uuids::Uuid_Float64,offsetof(struct MyStruct, number));

        // dummy simulation component to which add the field.
        std::unique_ptr<simdeck::Component> component(
            new simdeck::Component("testObj", "dummy object for testing", nullptr));
        // and its publication receiver.
        Publication pub(component.get(), &sim);

        // struct instance and its publication
        struct MyStruct structField={12, 42.42};
        auto f = dynamic_cast<Smp::IStructureField*>(pub.PublishField("structField", "", &structField, uuid,
                        Smp::ViewKind::VK_All, false, false, true));

        // check the returned field is OK.
        CPPUNIT_ASSERT(f != nullptr);
        auto child = component->GetField("structField");
        CPPUNIT_ASSERT(f == child);
        auto f1 = dynamic_cast<Smp::ISimpleField*>(f->GetField("word"));
        CPPUNIT_ASSERT(f1 != nullptr);
        auto f2 = dynamic_cast<Smp::ISimpleField*>(f->GetField("number"));
        CPPUNIT_ASSERT(f2 != nullptr);
        CPPUNIT_ASSERT(f1 != f2);
        auto f3 = f->GetChild("plop");
        CPPUNIT_ASSERT(f3 == nullptr);
        Smp::UInt16 w=f1->GetValue();
        CPPUNIT_ASSERT_EQUAL((Smp::UInt16)12, w);
        Smp::Float64 d=f2->GetValue();
        CPPUNIT_ASSERT_EQUAL((Smp::Float64)42.42, d);
    }
    ABS_TEST_CASE_END

    ABS_TEST_SUITE_END
}  // namespace test
