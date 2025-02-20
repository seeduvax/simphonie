/*
 * @file Publication.h
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_Publication_HPP__
#define __simphonie_kern_Publication_HPP__

#include <vector>
#include "Smp/DuplicateName.h"
#include "Smp/IPublication.h"
#include "Smp/Publication/ITypeRegistry.h"
#include "simdeck/Collection.hpp"

namespace simphonie {
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
    Smp::IObject* GetChild(Smp::String8 childName) const override;

    /**
     * @return the published object hold by this publication
// TODO is this still needed?
    inline Smp::IObject* getPubObj() const {
        return _pubObj;
    }
     */

    void addChild(IObject* pub);
    IObject* getChild(Smp::String8 name) const;
    void removeChild(Publication* pub);

    // Smp::IPublication implementation
    Smp::IField* PublishField(Smp::String8 name, Smp::String8 description, Smp::Char8* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    Smp::IField* PublishField(Smp::String8 name, Smp::String8 description, Smp::Bool* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    Smp::IField* PublishField(Smp::String8 name, Smp::String8 description, Smp::Int8* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    Smp::IField* PublishField(Smp::String8 name, Smp::String8 description, Smp::Int16* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    Smp::IField* PublishField(Smp::String8 name, Smp::String8 description, Smp::Int32* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    Smp::IField* PublishField(Smp::String8 name, Smp::String8 description, Smp::Int64* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    Smp::IField* PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt8* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    Smp::IField* PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt16* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    Smp::IField* PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt32* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    Smp::IField* PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt64* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    Smp::IField* PublishField(Smp::String8 name, Smp::String8 description, Smp::Float32* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    Smp::IField* PublishField(Smp::String8 name, Smp::String8 description, Smp::Float64* address,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    Smp::IField* PublishField(Smp::String8 name, Smp::String8 description, void* address, Smp::Uuid typeUuid,
                      Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true, Smp::Bool input = false,
                      Smp::Bool output = false) override;
    void PublishField(Smp::IField* field) override;
    Smp::ISimpleArrayField* PublishArray(Smp::String8 name, Smp::String8 description, Smp::Int64 count, void* address,
                      Smp::PrimitiveTypeKind type, Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true,
                      Smp::Bool input = false, Smp::Bool output = false) override;
    Smp::IPublication* PublishArray(Smp::String8 name, Smp::String8 description,
                                    Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true) override;
    Smp::IPublication* PublishStructure(Smp::String8 name, Smp::String8 description,
                                        Smp::ViewKind view = Smp::ViewKind::VK_All, Smp::Bool state = true) override;
    Smp::Publication::IPublishOperation* PublishOperation(
                    Smp::String8 name,
                    Smp::String8 description,
                    Smp::ViewKind view = Smp::ViewKind::VK_All) override;
    void PublishOperation(Smp::IOperation* operation) override;
    Smp::IProperty* PublishProperty(
                    Smp::String8 name,
                    Smp::String8 description,
                    Smp::Uuid typeUuid,
                    Smp::AccessKind accessKind,
                    Smp::ViewKind view = Smp::ViewKind::VK_All) override;
    void PublishProperty(Smp::IProperty* property) override;
    Smp::IField* GetField(Smp::String8 fullName) const override;
    const Smp::FieldCollection* GetFields() const override;
    Smp::IProperty* GetProperty(Smp::String8 name) const override;
    const Smp::PropertyCollection* GetProperties() const override;
    Smp::IOperation* GetOperation(Smp::String8 name) const override;
    const Smp::OperationCollection* GetOperations() const override;
    Smp::Publication::ITypeRegistry* GetTypeRegistry() const override;
    void Unpublish() override;

    void dump(int level = 0);

private:
    Smp::IComponent* _pubObj;
    std::vector<Smp::IObject*> _childs;
    Smp::Publication::ITypeRegistry* _typeRegistry;
    Smp::Publication::IType* getArrayType(Smp::PrimitiveTypeKind ptk, Smp::Int64 count);
    simdeck::Collection<Smp::IProperty> _properties;

    inline void addField(Smp::IField* field);
};

}  // namespace kern
}  // namespace simph
#endif  // __simphonie_kern_Publication_HPP__
