/*
 * @file EntryPointPublisher.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/EntryPointPublisher.hpp"

namespace simph {
namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
EntryPointPublisher::EntryPointPublisher() : _epList("epList", "", nullptr) {}
// ..........................................................
EntryPointPublisher::~EntryPointPublisher() {}

}  // namespace smpdk
}  // namespace simph
