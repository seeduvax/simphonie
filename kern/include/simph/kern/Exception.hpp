/*
 * @file Exception.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Exception_HPP__
#define __simph_kern_Exception_HPP__
#include <string>
#include "Smp/Exception.h"

namespace simph {
	namespace kern {

/**
 *
 */
class Exception: virtual public Smp::Exception {
public:
    /**
     * Default constructor.
     */
    Exception();
    /**
     * Destructor.
     */
    virtual ~Exception();

    Smp::String8 what() const noexcept override;
    Smp::String8 GetName() const noexcept override;
    Smp::String8 GetDescription() const noexcept override;
    const Smp::IObject* GetSender() const noexcept override;
    Smp::String8 GetMessage() const noexcept override;
protected:
    void setName(Smp::String8 name);
    void setDescription(Smp::String8 description);
    void setSender(const Smp::IObject* sender);
    void setMessage(Smp::String8 msg=nullptr);
private:
    std::string _name;
    std::string _description;
    const Smp::IObject* _sender;
    std::string _msg;

};

}} // namespace simph::kern
#endif // __simph_kern_Exception_HPP__
