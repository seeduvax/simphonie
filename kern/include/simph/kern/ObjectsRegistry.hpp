/*
 * @file ObjectsRegistry.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ObjectsRegistry_HPP__
#define __simph_kern_ObjectsRegistry_HPP__
#include "Smp/IPublication.h"
#include "Smp/Services/IResolver.h"
#include "simph/kern/Component.hpp"
#include "simph/kern/TypeRegistry.hpp"
#include <vector>

namespace Smp {
    class IEntryPoint;
    class IDataflowField;
}
namespace simph {
	namespace kern {
class Node;
/**
 *
 */
class ObjectsRegistry: virtual public Component,
                        virtual public Smp::Services::IResolver,
                        virtual public Smp::IPublication {
public:
    /**
     * Default constructor.
     */
    ObjectsRegistry(Smp::String8 name, Smp::String8 descr,
                    Smp::IObject* parent, TypeRegistry* typeRegistry);
    /**
     * Destructor.
     */
    virtual ~ObjectsRegistry();
    void dump(Smp::IObject* from=nullptr);

    void add(Smp::IObject* obj);
    const std::vector<Smp::IDataflowField*>* getRelatedFlowFields(const Smp::IEntryPoint* ep);
    // SMP::IResolver implementation.
    Smp::IObject* ResolveAbsolute(
                Smp::String8 absolutePath) override;
    Smp::IObject* ResolveRelative(
                Smp::String8 relativePath,
                const Smp::IComponent* sender) override;
    // Smp::IPublication implementation
    Smp::Publication::ITypeRegistry* GetTypeRegistry() const override;
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Char8* address,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false) override;
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Bool* address,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false) override;
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int8* address,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false) override;
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int16* address,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false) override;
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int32* address,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false) override;
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int64* address,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false) override;
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt8* address,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false) override;
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt16* address,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false) override;
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt32* address,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false) override;
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt64* address,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false) override;
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Float32* address,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false) override;
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Float64* address,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false) override;
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        void* address,
        Smp::Uuid typeUuid,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false) override;
    void PublishField(Smp::IField* field) override;
    void PublishArray(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int64 count,
        void* address,
        Smp::PrimitiveTypeKind type,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false) override;
    Smp::IPublication* PublishArray(
        Smp::String8 name,
        Smp::String8 description,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true) override;
    Smp::IPublication* PublishStructure(
        Smp::String8 name,
        Smp::String8 description,
        Smp::ViewKind view=Smp::ViewKind::VK_All,
        Smp::Bool state=true) override;
    Smp::Publication::IPublishOperation* PublishOperation(
        Smp::String8 name,
        Smp::String8 description,
        Smp::ViewKind view=Smp::ViewKind::VK_None) override;
    void PublishProperty(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Uuid typeUuid,
        Smp::AccessKind accessKind,
        Smp::ViewKind view=Smp::ViewKind::VK_None) override;
    void Unpublish() override;
    Smp::IField* GetField(Smp::String8 fullName) const override;
    const Smp::FieldCollection* GetFields() const override;
    Smp::IRequest* CreateRequest(Smp::String8 operationName) override;
    void DeleteRequest(Smp::IRequest* request) override;
private:
    Smp::Publication::IType* getArrayType(Smp::PrimitiveTypeKind ptk,Smp::Int64 count);
    Node* _root;
    Node* _currentNode;
    Node* findNode(const Smp::IObject* obj);
    TypeRegistry* _typeRegistry;
    Smp::IObject* resolve(Node* from,Smp::String8 path);
    void addField(Smp::IField* field);
    Smp::IObject* getFieldParent();
};

}} // namespace simph::kern
#endif // __simph_kern_ObjectsRegistry_HPP__
