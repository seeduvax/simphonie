/*
 * @file ComponentWrap.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ComponentWrap_HPP__
#define __simph_kern_ComponentWrap_HPP__
#include "simph/kern/Component.hpp"
#include "Smp/IPublication.h"

namespace simph {
	namespace kern {

/**
 *
 */
class ComponentWrap: virtual public Component, 
                    virtual public Smp::IPublication {
public:
    /**
     * Default constructor.
     */
    ComponentWrap(Smp::Publication::ITypeRegistry* tReg);
    /**
     * Destructor.
     */
    virtual ~ComponentWrap();
    // Smp::IPublication implementation
    Smp::Publication::ITypeRegistry* GetTyperegistry() const;
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Char* address,
        Smp::viewKind view=ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false);
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Bool* address,
        Smp::viewKind view=ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false);
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int8* address,
        Smp::viewKind view=ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false);
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int16* address,
        Smp::viewKind view=ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false);
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int32* address,
        Smp::viewKind view=ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false);
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int64* address,
        Smp::viewKind view=ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false);
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt8* address,
        Smp::viewKind view=ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false);
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt16* address,
        Smp::viewKind view=ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false);
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt32* address,
        Smp::viewKind view=ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false);
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt64* address,
        Smp::viewKind view=ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false);
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Float32* address,
        Smp::viewKind view=ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false);
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Float64* address,
        Smp::viewKind view=ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false);
    void PublishField(
        Smp::String8 name,
        Smp::String8 description,
        void* address,
        Smp::Uuid typeUuid,
        Smp::viewKind view=ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false);
    void PublishField(Smp::IField* field);
    void PublishArray(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int64 count,
        void* address,
        Smp::PrimitiveTypeKind type,
        Smp::viewKind view=ViewKind::VK_All,
        Smp::Bool state=true,
        Smp::Bool input=false,
        Smp::Bool output=false);
    Smp::IPublication* PublishArray(
        Smp::String8 name,
        Smp::String8 description,
        Smp::ViewKind view=ViewKind::VK_All,
        Smp::Bool state=true);
    Smp::IPublication* PublishStructure(
        Smp::String8 name,
        Smp::String8 description,
        Smp::ViewKind view=ViewKind::VK_All,
        Smp::Bool state=true);
    Smp::Publication::IPublicationOperation* PublishOperation(
        Smp::String8 name,
        Smp::String8 description,
        Smp::ViewKind view=ViewKind::VK_None);
    void PublishProperty(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Uuid typeUuid,
        Smp::AccessKind accessKind,
        Smp::ViewKind view=ViewKind::VK_None);
    Smp::IRequest* CreateRequest(Smp::String8 operationName);
    void DeleteRequest(Smp::IRequest* request);


private:

};

}} // namespace simph::kern
#endif // __simph_kern_ComponentWrap_HPP__
