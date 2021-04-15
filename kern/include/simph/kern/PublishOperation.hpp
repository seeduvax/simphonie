/*
 * @file PublishOperation.hpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_PublishOperation_HPP__
#define __simph_kern_PublishOperation_HPP__

#include "Smp/Publication/IPublishOperation.h"

namespace simph {
namespace kern {

/**
 *
 */
class PublishOperation : virtual Smp::Publication::IPublishOperation {
public:
    /**
     * Default constructor.
     */
    PublishOperation();
    /**
     * Destructor.
     */
    virtual ~PublishOperation();

    // Smp::Publication::IPublishOperation
    void PublishParameter(
        Smp::String8 name, Smp::String8 description, Smp::Uuid typeUuid,
        Smp::Publication::ParameterDirectionKind direction = Smp::Publication::ParameterDirectionKind::PDK_In) override;

private:
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_PublishOperation_HPP__
