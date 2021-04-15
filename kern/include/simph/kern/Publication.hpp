/*
 * @file Publication.h
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Publication_HPP__
#define __simph_kern_Publication_HPP__

#include <vector>
#include "Smp/DuplicateName.h"
#include "Smp/IPublication.h"
#include "Smp/Publication/ITypeRegistry.h"
#include "simph/smpdk/Collection.hpp"

namespace simph {
namespace kern {

/**
 *
 */
class Publication : virtual public Smp::IObject, virtual public Smp::IPublication {
public:
    /**
     * Default constructor.
     */
    Publication(Smp::IObject* toPublish, Smp::Publication::ITypeRegistry* typeRegistry);
    /**
     * Destructor.
     */
    virtual ~Publication();

    // Smp::IObject implementation
    Smp::String8 GetName() const override;
    Smp::String8 GetDescription() const override;
    Smp::IObject* GetParent() const override;

    /**
     * @return the published object hold by this publication
     */
    inline Smp::IObject* getPubObj() const {
        return _pubObj;
    }

    void addChild(IObject* pub);
    IObject* getChild(Smp::String8 name) const;
    void removeChild(Publication* pub);

    // Smp::IPublication implementation
    Smp::Publication::ITypeRegistry* GetTypeRegistry() const override;
    void PublishField(Smp::String8 name, Smp::String8 description, Smp::Char8* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    void PublishField(Smp::String8 name, Smp::String8 description, Smp::Bool* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    void PublishField(Smp::String8 name, Smp::String8 description, Smp::Int8* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    void PublishField(Smp::String8 name, Smp::String8 description, Smp::Int16* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    void PublishField(Smp::String8 name, Smp::String8 description, Smp::Int32* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    void PublishField(Smp::String8 name, Smp::String8 description, Smp::Int64* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    void PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt8* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    void PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt16* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    void PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt32* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    void PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt64* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    void PublishField(Smp::String8 name, Smp::String8 description, Smp::Float32* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    void PublishField(Smp::String8 name, Smp::String8 description, Smp::Float64* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    void PublishField(Smp::String8 name, Smp::String8 description, void* address, Smp::Uuid typeUuid,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    void PublishField(Smp::IField* field) override;
    void PublishArray(Smp::String8 name, Smp::String8 description, Smp::Int64 count, void* address,
                      Smp::PrimitiveTypeKind type, Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true,
                      Smp::Bool input = false, Smp::Bool output = false) override;
    Smp::IPublication* PublishArray(Smp::String8 name, Smp::String8 description,
                                    Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true) override;
    Smp::IPublication* PublishStructure(Smp::String8 name, Smp::String8 description,
                                        Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true) override;
    Smp::Publication::IPublishOperation* PublishOperation(Smp::String8 name, Smp::String8 description,
                                                          Smp::ViewKind view = Smp::ViewKind::VK_None) override;
    void PublishProperty(Smp::String8 name, Smp::String8 description, Smp::Uuid typeUuid, Smp::AccessKind accessKind,
                         Smp::ViewKind view = Smp::ViewKind::VK_None) override;
    void Unpublish() override;
    Smp::IField* GetField(Smp::String8 fullName) const override;
    const Smp::FieldCollection* GetFields() const override;
    const Smp::PropertyCollection* GetProperties() const override;
    const Smp::OperationCollection* GetOperations() const override;
    Smp::IRequest* CreateRequest(Smp::String8 operationName) override;
    void DeleteRequest(Smp::IRequest* request) override;

    void dump(int level = 0);

private:
    Smp::IObject* _pubObj;
    std::vector<Smp::IObject*> _childs;
    Smp::Publication::ITypeRegistry* _typeRegistry;
    Smp::Publication::IType* getArrayType(Smp::PrimitiveTypeKind ptk, Smp::Int64 count);
    simph::smpdk::Collection<Smp::IField> _fields;
    simph::smpdk::Collection<Smp::IProperty> _properties;

    inline void addField(Smp::IField* field) {
        addChild(field);
        _fields.push_back(field);
    }
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_Publication_HPP__
