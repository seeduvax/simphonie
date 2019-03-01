/*
 * @file Generator1D.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/umdl/Generator1D.hpp"

namespace simph {
	namespace umdl {
// --------------------------------------------------------------------
// ..........................................................
Generator1D::Generator1D(Smp::String8 name, Smp::String8 descr,
                        Smp::IObject* parent):
                Component(name,descr,parent),
                AStepMdl(name,descr,parent) {
}
// ..........................................................
Generator1D::~Generator1D() {
}
// --------------------------------------------------------------------
// ..........................................................
void Generator1D::publish(Smp::IPublication* receiver) {
    receiver->PublishField("out","Output",&_out,Smp::ViewKind::VK_All,
                                                    false,false,true);
}
// ..........................................................
void Generator1D::step() {
    _out=compute();
}

}} // namespace simph::umdl
