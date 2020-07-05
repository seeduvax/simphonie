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

#include "Smp/IPublication.h"
#include "Smp/DuplicateName.h"
#include "simph/smpdk/Object.hpp"
#include <vector>

namespace simph {
	namespace kern {

/**
 *
 */
class Publication: virtual public Smp::IObject {
public:
    /**
     * Default constructor.
     */
    Publication(Smp::IObject* toPublish, Smp::IObject* parent);
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

    void addChild(Publication* pub) throw (Smp::DuplicateName);
    Publication* getChild(Smp::String8 name) const;
    void removeChild(Publication* pub);

private:
    Smp::IObject* _pubObj;
    Smp::IObject* _parent;
    std::vector<Publication*> _childs;
};

}} // namespace simph::kern
#endif // __simph_kern_Publication_HPP__
