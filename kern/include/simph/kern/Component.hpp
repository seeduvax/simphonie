/*
 * @file Component.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Component_HPP__
#define __simph_kern_Component_HPP__
#include "simph/kern/Object.hpp"
#include "simph/kern/Collection.hpp"
#include "Smp/IComponent.h"
#include "Smp/Services/ILogger.h"

namespace simph {
	namespace kern {

/**
 *
 */
class Component: public Object, virtual public Smp::IComponent {
public:
    /**
     * Default constructor.
     */
    Component(Smp::String8 name,
                Smp::String8 descr="",
                Smp::IObject* parent=nullptr);
    /**
     * Destructor.
     */
    virtual ~Component();

    // Smp::IComponent implementation
    Smp::ComponentStateKind GetState() const;
    void Publish(Smp::IPublication* receiver);
    void Configure(Smp::Services::ILogger* logger);
    void Connect(Smp::ISimulator* simulator);
    Smp::IField* GetField(Smp::String8 fullName) const;
    const Smp::FieldCollection* GetFields() const;
    const Smp::Uuid& GetUuid() const;
protected:
    inline Smp::ISimulator* getSimulator() const {
        return _simulator;
    }
    virtual void publish(Smp::IPublication* receiver);
    virtual void configure();
    virtual void connect();
    inline void addField(Smp::IField* field) {
        _fields.push_back(field);
    }
    inline void logDebug(Smp::String8 msg) {
        _logger->Log(this,msg,Smp::Services::ILogger::LMK_Debug);
    }
    inline void logInfo(Smp::String8 msg) {
        _logger->Log(this,msg,Smp::Services::ILogger::LMK_Information);
    }
    inline void logWarning(Smp::String8 msg) {
        _logger->Log(this,msg,Smp::Services::ILogger::LMK_Warning);
    }
    inline void logError(Smp::String8 msg) {
        _logger->Log(this,msg,Smp::Services::ILogger::LMK_Error);
    }
    inline void logEvent(Smp::String8 msg) {
        _logger->Log(this,msg,Smp::Services::ILogger::LMK_Event);
    }
private:
    Smp::ComponentStateKind _state;
    Smp::Services::ILogger* _logger;
    Smp::ISimulator* _simulator;
    Collection<Smp::IField> _fields;
    Smp::Uuid _uuid;
};

}} // namespace simph::kern
#endif // __simph_kern_Component_HPP__
