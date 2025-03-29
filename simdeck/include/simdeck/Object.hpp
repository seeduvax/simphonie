/*
 * @file Object.h
 *
 * Copyright 2019 S.Devaux . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_Object_HPP__
#define __simdeck_Object_HPP__
#include <string>
#include "simdeck/Uuid.hpp"
#include "Smp/IObject.h"

namespace simdeck {

/**
 *
 */
class Object : public virtual Smp::IObject {
public:
    /**
     * Default constructor.
     */
    Object(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr, Smp::Bool overrideNameCheck = false);
    /**
     * Destructor.
     */
    virtual ~Object();

    Smp::String8 GetName() const override;
    Smp::String8 GetDescription() const override;
    Smp::IObject* GetParent() const override;
    /**
     * Get child object by name.
     * Default implementation is a leaf object case, meaning an object having
     * no child. Composit object shall overide this method to search for
     * the child object among the containers.
     * @param name name of searched child
     * @return nullptr
     */
    Smp::IObject* GetChild(Smp::String8 name) const override;


protected:
    void checkName(Smp::String8 name);
    static Smp::IObject* resolveChild(Smp::String8 name, const Smp::IObject* from);
private:
    std::string _name;
    std::string _description;
    Smp::IObject* _parent;
};

}  // namespace simdeck
#endif  // __simdeck_Object_HPP__
