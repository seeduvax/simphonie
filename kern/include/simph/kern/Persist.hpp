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
#include "simph/smpdk/Object.hpp"
namespace simph {
namespace kern {
using namespace simph::smpdk;

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

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_Persist_HPP__
