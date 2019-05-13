/*
 * @file Object.h
 *
 * Copyright 2019 S.Devaux . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Object_HPP__
#define __simph_kern_Object_HPP__
#include "Smp/IObject.h"
#include <string>

namespace simph {
	namespace kern {

/**
 *
 */
class Object: public virtual Smp::IObject {
public:
    /**
     * Default constructor.
     */
    Object(Smp::String8 name, 
            Smp::String8 descr="",
            Smp::IObject* parent=nullptr);
    /**
     * Destructor.
     */
    virtual ~Object();

    Smp::String8 GetName() const override;
    Smp::String8 GetDescription() const override;
    Smp::IObject* GetParent() const override;

    static bool checkName(Smp::String8 name);
protected:

private:
    std::string _name;
    std::string _description;
    Smp::IObject* _parent;

    // TODO reconsider to make the 3 following protected if really needed.
    void setName(Smp::String8 name);
    void setDescription(Smp::String8 description);
    void setParent(Smp::IObject* parent);
};

}} // namespace simph::kern
#endif // __simph_kern_Object_HPP__
