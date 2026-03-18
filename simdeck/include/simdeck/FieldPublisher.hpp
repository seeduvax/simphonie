/*
 * @file FieldPublisher.hpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_FieldPublisher_HPP__
#define __simdeck_FieldPublisher_HPP__

#include "Smp/IPublication.h"
#include "simdeck/Collection.hpp"
#include "Smp/IPublication.h"
#include "Smp/IObject.h"
#include "simdeck/Collection.hpp"
#include "Smp/Publication/ITypeRegistry.h"

namespace simdeck {

/**
 *
 */
class FieldPublisher :  public Smp::IPublication {
public:
    /**
     * Default constructor.
     */
    FieldPublisher(Smp::IObject*  pubObj);
    /**
     * Destructor.
     */
    virtual ~FieldPublisher();

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


private:
    Smp::IObject* _pubObj;
    inline void addField(Smp::IField* field);
    OwnedCollection<Smp::IField> _published;

};

} // namespace simdeck
#endif // __simdeck_FieldPublisher_HPP__
