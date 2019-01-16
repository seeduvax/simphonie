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
    Object();
    /**
     * Destructor.
     */
    virtual ~Object();

    Smp::String8 GetName() const;
    Smp::String8 GetDescription() const;
    Smp::IObject* GetParent() const;

protected:
    void setName(Smp::String8 name);
    void setDescription(Smp::String8 description);
    void setParent(Smp::IObject* parent);

private:
    std::string _name;
    std::string _description;
    Smp::IObject* _parent;
    bool checkName();

};

}} // namespace simph::kern
#endif // __simph_kern_Object_HPP__
