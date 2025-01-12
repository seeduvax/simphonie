/*
 * @file IEntryPointPublisher.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/IEntryPointPublisher.hpp"

namespace simph {
namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
IEntryPointPublisher::IEntryPointPublisher() : _epList("epList", "", nullptr) {}
// ..........................................................
IEntryPointPublisher::~IEntryPointPublisher() {}

}  // namespace smpdk
}  // namespace simph
