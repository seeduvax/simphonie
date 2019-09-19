/*
 * @file TypeRegistry.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_TypeRegistry_HPP__
#define __simph_kern_TypeRegistry_HPP__
#include "simph/kern/Component.hpp"
#include "simph/kern/Collection.hpp"
#include "Smp/Publication/ITypeRegistry.h"

namespace simph {
	namespace kern {

/**
 *
 */
class TypeRegistry: public Component,
                virtual public Smp::Publication::ITypeRegistry {
public:
    /**
     * Default constructor.
     */
    TypeRegistry(Smp::String8 name, Smp::String8 descr,
                    Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~TypeRegistry();
    /**
     * Get the byte size of a primitive type.
     * @param type primitive type to query for byte size 
     * @return byte size of the provided primitive type
     */
    static size_t getPrimitiveTypeSize(Smp::PrimitiveTypeKind type);
    /**
     * Get the name of a primitive type.
     * @param type primitive type to use to fetch for a name
     * @return name of the provided primitive type
     */
    static Smp::String8 getPrimitiveTypeName(Smp::PrimitiveTypeKind _type);
    // Smp::ITypeRegistry implementation
    Smp::Publication::IType* GetType(Smp::PrimitiveTypeKind type) const override;
    Smp::Publication::IType* GetType(Smp::Uuid typeUuid) const override;
    Smp::Publication::IType* AddFloatType(
            Smp::String8 name,
            Smp::String8 description,
            Smp::Uuid typeUuid,
            Smp::Float64 minimum,
            Smp::Float64 maximum,
            Smp::Bool minInclusive,
            Smp::Bool maxInclusive,
            Smp::String8 unit,
            Smp::PrimitiveTypeKind type=Smp::PrimitiveTypeKind::PTK_Float64) override;
    Smp::Publication::IType* AddIntegerType(
            Smp::String8 name,
            Smp::String8 description,
            Smp::Uuid typeUuid,
            Smp::Int64 minimum,
            Smp::Int64 maximum,
            Smp::String8 unit,
            Smp::PrimitiveTypeKind type=Smp::PrimitiveTypeKind::PTK_Int32) override;
    Smp::Publication::IEnumerationType* AddEnumerationType(
            Smp::String8 name,
            Smp::String8 descr,
            Smp::Uuid typeUuid,
            Smp::Int16 memorySize) override;
    Smp::Publication::IArrayType* AddArrayType(
            Smp::String8 name,
            Smp::String8 description,
            Smp::Uuid typeUuid,
            Smp::Uuid itemTypeUuid,
            Smp::Int64 itemSize,
            Smp::Int64 arrayCount) override;
    Smp::Publication::IType* AddStringType(
            Smp::String8 name,
            Smp::String8 description,
            Smp::Uuid typeUuid,
            Smp::Int64 length) override;
    Smp::Publication::IStructureType* AddStructureType(
            Smp::String8 name,
            Smp::String8 description,
            Smp::Uuid typeUuid) override;
    Smp::Publication::IClassType* AddClassType(
            Smp::String8 name,
            Smp::String8 description,
            Smp::Uuid typeUuid,
            Smp::Uuid baseClassUuid) override;


private:
    Collection<Smp::Publication::IType> _types;
};

}} // namespace simph::kern
#endif // __simph_kern_TypeRegistry_HPP__
