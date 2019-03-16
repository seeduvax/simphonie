/*
 * @file IEntryPointPublisher.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/IEntryPointPublisher.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
IEntryPointPublisher::IEntryPointPublisher(): _epList("epList","",nullptr) {
}
// ..........................................................
IEntryPointPublisher::~IEntryPointPublisher() {
}

}} // namespace simph::kern
