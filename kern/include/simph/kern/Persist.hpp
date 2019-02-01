/*
 * @file Persist.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Persist_HPP__
#define __simph_kern_Persist_HPP__
#include "Smp/IPersist.h"
#include "simph/kern/Object.hpp"
namespace simph {
	namespace kern {

/**
 *
 */
class Persist: virtual public Object, virtual public Smp::IPersist {
public:
    /**
     * Default constructor.
     */
    Persist();
    /**
     * Destructor.
     */
    virtual ~Persist();
    // Smp::IPersist implementation
    void Restore(Smp::IStorageReader* reader);
    void Store(Smp::IStorageWriter* writer);
private:

};

}} // namespace simph::kern
#endif // __simph_kern_Persist_HPP__
