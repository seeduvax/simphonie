/*
 * @file Factory.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_T_kern_Factory_HPP__
#define __simph_T_kern_Factory_HPP__
#include "simba/SmpModelLibFactory.hpp"


namespace simph {
	namespace T_kern {

class Factory: public simba::SmpModelLibFactory {
private:

public:
        /**
         * Default constructor.
         */
        Factory(Smp::IDynamicSimulator* sim,Smp::Publication::ITypeRegistry* reg);
        /**
         * Destructor.
         */
        virtual ~Factory();
        

};

}} // namespace simph::T_kern
#endif // __simph_T_kern_Factory_HPP__
