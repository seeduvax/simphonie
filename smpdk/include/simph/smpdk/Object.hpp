/*
 * @file Object.h
 *
 * Copyright 2019 S.Devaux . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_Object_HPP__
#define __simph_smpdk_Object_HPP__
#include <string>
#include "Smp/IObject.h"

namespace simph {
namespace smpdk {

/**
 *
 */
class Object : public virtual Smp::IObject {
public:
    /**
     * Default constructor.
     */
    Object(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
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
private:
    std::string _name;
    std::string _description;
    Smp::IObject* _parent;
};

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_Object_HPP__
