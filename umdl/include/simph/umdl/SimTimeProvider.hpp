/*
 * @file SimTimeProvider.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_umdl_SimTimeProvider_HPP__
#define __simph_umdl_SimTimeProvider_HPP__
#include "simph/umdl/Generator1D.hpp"

namespace simph {
	namespace umdl {

/**
 *
 */
class SimTimeProvider: virtual public Generator1D {
public:
    /**
     * Default constructor.
     */
    SimTimeProvider(Smp::String8 name, Smp::String8 descr,
                    Smp::IObject* parent=nullptr);
    /**
     * Destructor.
     */
    virtual ~SimTimeProvider();
protected:
    Smp::Float64 compute();
private:

};

}} // namespace simph::umdl
#endif // __simph_umdl_SimTimeProvider_HPP__
