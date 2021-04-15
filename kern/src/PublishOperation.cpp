/*
 * @file PublishOperation.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/PublishOperation.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
PublishOperation::PublishOperation() {}
// ..........................................................
PublishOperation::~PublishOperation() {}
// --------------------------------------------------------------------
// ..........................................................
void PublishOperation::PublishParameter(Smp::String8 name, Smp::String8 description, Smp::Uuid typeUuid,
                                        Smp::Publication::ParameterDirectionKind direction) {
    // TODO
    LOGE("PublishOperation::PublishParameter not implemented yet !!!");
}

}  // namespace kern
}  // namespace simph
