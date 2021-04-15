/*
 * @file EntryPoint.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/EntryPoint.hpp"

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
EntryPoint::EntryPoint(std::unique_ptr<Runnable> runnable, Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Object(name, descr, parent), _runnable(std::move(runnable)) {}
// ..........................................................
EntryPoint::~EntryPoint() {}

}  // namespace kern
}  // namespace simph
