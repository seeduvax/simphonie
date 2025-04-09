/*
 * @file Component.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_Component_HPP__
#define __simdeck_Component_HPP__

#include "Smp/IComponent.h"
#include "Smp/Services/ILinkRegistry.h"
#include "Smp/Services/ILogger.h"
#include "simdeck/Collection.hpp"
#include "simdeck/Object.hpp"

namespace simdeck {

/**
 *
 */
class Component : public Object, virtual public Smp::IComponent {
public:
    /**
     * Default constructor.
     */
    Component(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~Component();

    // Smp::IComponent implementation
    Smp::ComponentStateKind GetState() const override;
    void Publish(Smp::IPublication* receiver) override;
    void Configure(Smp::Services::ILogger* logger, Smp::Services::ILinkRegistry* linkRegistry = nullptr) override;
    void Connect(Smp::ISimulator* simulator) override;
    void Disconnect() override;
    Smp::IField* GetField(Smp::String8 fullName) const override;
    const Smp::FieldCollection* GetFields() const override;
    const Smp::Uuid& GetUuid() const override;
    Smp::AnySimple GetSimpleValue(Smp::String8 fullName) const override;
    void SetSimpleValue(
                Smp::String8 fullName,
                Smp::AnySimple value) override;
    void GetSimpleArrayValue(
                Smp::String8 fullName,
                Smp::UInt64 length,
                Smp::AnySimple* values,
                Smp::UInt64 startIndex = 0) const override;
    void SetSimpleArrayValue(
                Smp::String8 fullName,
                Smp::UInt64 length,
                Smp::AnySimpleArray values,
                Smp::UInt64 startIndex = 0) override;
    Smp::Bool AddChild(
                Smp::IObject* child,
                const Smp::ICollectionBase* collection) override;
    Smp::Bool RemoveChild(
                Smp::IObject* child,
                const Smp::ICollectionBase* collection) override;
    Smp::IObject* IsChildInCollection(
                Smp::String8 child,
                const Smp::ICollectionBase* collection) const override;

protected:
    Smp::ISimulator* getSimulator();

    virtual void publish(Smp::IPublication* receiver);
    virtual void configure();
    virtual void connect();
    virtual void disconnect();
    inline void addField(Smp::IField* field) {
        _fields.push_back(field);
    }
    inline void logDebug(Smp::String8 msg) {
        _logger->Log(this, msg, Smp::Services::ILogger::LMK_Debug);
    }
    inline void logInfo(Smp::String8 msg) {
        _logger->Log(this, msg, Smp::Services::ILogger::LMK_Information);
    }
    inline void logWarning(Smp::String8 msg) {
        _logger->Log(this, msg, Smp::Services::ILogger::LMK_Warning);
    }
    inline void logError(Smp::String8 msg) {
        _logger->Log(this, msg, Smp::Services::ILogger::LMK_Error);
    }
    inline void logEvent(Smp::String8 msg) {
        _logger->Log(this, msg, Smp::Services::ILogger::LMK_Event);
    }
    inline Smp::Services::ILinkRegistry* getLinkRegistry() {
        return _linkRegistry;
    }

private:
    Smp::ComponentStateKind _state;
    Smp::Services::ILogger* _logger;
    Smp::Services::ILinkRegistry* _linkRegistry;
    OwnedCollection<Smp::IField> _fields;
    Smp::Uuid _uuid;
    Smp::ISimulator* _simulator;
};

}  // namespace simdeck
#endif  // __simdeck_Component_HPP__
