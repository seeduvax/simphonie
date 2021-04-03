/*
 * @file Factory.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/T_kern/Factory.hpp"
#include "simph/T_kern/SmpIncrement.hpp"

namespace simph {
	namespace T_kern {

Factory::Factory(Smp::IDynamicSimulator* sim,Smp::Publication::ITypeRegistry* reg):
				SmpModelLibFactory(sim,reg,"simph_T_kern") {
	registerModelType<SmpIncrement>("SmpIncrement","SmpIncrement model");
}
// ..........................................................
Factory::~Factory() {
}

}} // namespace simph::T_kern

DEFINE_SMP_LIBINIT(simph::T_kern::Factory)