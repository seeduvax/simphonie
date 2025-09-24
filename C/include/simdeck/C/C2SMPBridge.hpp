/*
 * @file C2SMPBridge.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_C2SMPBridge_HPP__
#define __simphonie_kern_C2SMPBridge_HPP__
#include "Smp/IModel.h"
#include "Smp/Exception.h"
#include "Smp/Services/ILogger.h"
#include "simdeck/Exception.hpp"
#include "simdeck/Model.hpp"
#include "simdeck/Composite.hpp"
#include "simdeck/EntryPoint.hpp"
#include "simdeck/Collection.hpp"
#include "CModel.h"
#include "CWrapper.h"
#include <sstream>

#define TO_STR(a) #a
#define CHECK_RETURN(name, ret)                                                                       \
    if (ret > 0) {                                                                                    \
        std::ostringstream msg;                                                                       \
        msg << TO_STR(C model name failed: returned) << ret;                                          \
        getSimulator()->GetLogger()->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Error); \
        throw new simdeck::Exception(this, msg.str().c_str());                                        \
    }

#define CW_Smp CW_BUILD_SPACE(Smp)
#define CW_SPACE CW_BUILD_SPACE(CW_Smp, Services)


/* DEFINITIONS */

namespace simphonie {
namespace kern {

/**
 * @brief Enables C models within the SMP framework.
 *
 * @warning Here as a POC, numerous features are missing.
 *
 * @tparam The function for initializing the CModel object. TODO change Factory to allow creating component with custom arguments to prevent the use of template
 * 
 * TODO: add container
 * TODO: complete CWrapper with every single public function of at least Smp
 */
template <int (*Init)(CModel* model, const SmpEnv* smpEnv)>
class C2SMPBridge : public simdeck::Model, virtual public simdeck::AComposite, virtual public Smp::IEntryPointPublisher {
public:
    C2SMPBridge(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent = nullptr);
    ~C2SMPBridge() = default;

    inline void addEPFromCModel(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent, CModelEP ep) {
        _eps.push_back(new EntryPoint(ep, _model.env, &_env, name, descr, this));
    }

    /* Smp::IEntryPointPublisher */
    inline const Smp::EntryPointCollection* GetEntryPoints() const override { return &_eps; }

    /* Smp::IEntryPointPublisher */
    inline Smp::IEntryPoint* GetEntryPoint(Smp::String8 name) const override { return _eps.at(name); }

private:
    class EntryPoint: public simdeck::EntryPoint {
    public:
        /**
         * Default constructor.
         */
        inline EntryPoint(CModelEP ep, void* env, const SmpEnv* smpEnv, Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr)
            : simdeck::EntryPoint(name, descr, parent), _ep(ep), _env(env), _smpEnv(smpEnv)
        {}

        /**
         * Destructor.
         */
        ~EntryPoint() = default;

        /* Smp::IEntryPoint */
        inline void Execute() const override { (*_ep)(_env, _smpEnv); }

    private:
        CModelEP _ep;
        void* _env;
        const SmpEnv* _smpEnv;
    };

    void publish(Smp::IPublication* receiver) override;
    void configure() override;
    void connect() override;
    static int addEntryPoint(Bridge* bridge, const char* name, const char* description, CModelEP ep);
    static int publishField(Bridge* bridge, const char* name, const char* description, void* pointer, bool input, bool output);
    static CW(ILogger)* getLogger(Bridge* bridge);
    static CW(IEventManager)* getEventManager(Bridge* bridge);

    CModel _model;
    const SmpEnv _env = { &addEntryPoint, &publishField, &getLogger, &getEventManager, this };
    Smp::IPublication* _receiver;
    simdeck::OwnedCollection<Smp::IEntryPoint> _eps;
};

} /* namespace kern */
} /* namespace simphonie */


/* IMPLEMENTATIONS */
// --------------------------------------------------------------------
// ..........................................................
template <int (*Init)(CModel* model, const SmpEnv* smpEnv)>
simphonie::kern::C2SMPBridge<Init>::C2SMPBridge(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent) :
    simdeck::Model(name, descr, parent)
{
    const auto res = Init(&_model, &_env);
    CHECK_RETURN(instantiation, res)
}
// ..........................................................
template <int (*Init)(CModel* model, const SmpEnv* smpEnv)>
void simphonie::kern::C2SMPBridge<Init>::publish(Smp::IPublication* receiver) {
    _receiver = receiver;
    const auto res = _model.publish(_model.env, &_env);
    CHECK_RETURN(publication, res)
}
// ..........................................................
template <int (*Init)(CModel* model, const SmpEnv* smpEnv)>
void simphonie::kern::C2SMPBridge<Init>::configure() {
    const auto res = _model.configure(_model.env, &_env);
    CHECK_RETURN(configure, res)
}
// ..........................................................
template <int (*Init)(CModel* model, const SmpEnv* smpEnv)>
void simphonie::kern::C2SMPBridge<Init>::connect() {
    const auto res = _model.connect(_model.env, &_env);
    CHECK_RETURN(connection, res)
}
// ..........................................................
template <int (*Init)(CModel* model, const SmpEnv* smpEnv)>
int simphonie::kern::C2SMPBridge<Init>::addEntryPoint(Bridge* bridge, const char* name, const char* description, CModelEP ep) {
    auto self = static_cast<C2SMPBridge*>(bridge);
    if (self == nullptr) return 1;
    self->addEPFromCModel(name, "Handler for a C models's entrypoint", self, ep);
    return 0;
}
// ..........................................................
template <int (*Init)(CModel* model, const SmpEnv* smpEnv)>
int simphonie::kern::C2SMPBridge<Init>::publishField(Bridge* bridge, const char* name, const char* description, void* pointer, bool input, bool output) {
    const auto b = reinterpret_cast<C2SMPBridge*>(bridge);
    if (b == nullptr) return 1;
    const auto res = b->_receiver->PublishField(name, description, pointer, Smp::Uuids::Uuid_Float64, Smp::ViewKind::VK_All, false, input, output); /* TODO Uuid */
    if (res == nullptr) return 1;
    return 0;
}
// ..........................................................
template <int (*Init)(CModel* model, const SmpEnv* smpEnv)>
CW(ILogger)* simphonie::kern::C2SMPBridge<Init>::getLogger(Bridge* bridge) {
    auto b = reinterpret_cast<C2SMPBridge*>(bridge);
    if (b == nullptr) return NULL;
    return b->getSimulator()->GetLogger();
}
// ..........................................................
template <int (*Init)(CModel* model, const SmpEnv* smpEnv)>
CW(IEventManager)* simphonie::kern::C2SMPBridge<Init>::getEventManager(Bridge* bridge) {
    auto b = reinterpret_cast<C2SMPBridge*>(bridge);
    if (b == nullptr) return NULL;
    return b->getSimulator()->GetEventManager();
}

#undef CW_SPACE
#undef CW_Smp
#undef CHECK_RETURN
#undef TO_STR

#endif /* __simphonie_kern_C2SMPBridge_HPP__ */
