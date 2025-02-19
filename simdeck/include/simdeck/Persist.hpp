/*
 * @file Persist.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_Persist_HPP__
#define __simdeck_Persist_HPP__
#include "Smp/IPersist.h"
#include "simdeck/Object.hpp"
namespace simdeck {

/**
 *
 */
class Persist : public Object, virtual public Smp::IPersist {
public:
    /**
     * Default constructor.
     */
    Persist(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~Persist();
    // Smp::IPersist implementation
    void Restore(Smp::IStorageReader* reader) override;
    void Store(Smp::IStorageWriter* writer) override;

private:
};

}
#endif  // __simdeck_Persist_HPP__
