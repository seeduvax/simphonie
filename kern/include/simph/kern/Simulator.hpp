/*
 * @file Simulator.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Simulator_HPP__
#define __simph_kern_Simulator_HPP__
#include "Smp/ISimulator.h"
#include "Smp/Publication/ITypeRegistry.h"
#include "simph/smpdk/Composite.hpp"
#include "simph/sys/DLib.hpp"

namespace simph {
namespace kern {
using namespace simph::smpdk;

class Resolver;
class Scheduler;
/**
 *
 */
class Simulator : public Composite, public virtual Smp::ISimulator {
public:
    /**
     * Default constructor.
     */
    Simulator(Smp::String8 name = "Simphonie", Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~Simulator();
    // Smp::ISimulator implementation
    void Initialise() override;
    void Publish() override;
    void Configure() override;
    void Connect() override;
    void Run() override;
    void Hold(Smp::Bool immediate) override;
    void Store(Smp::String8 filename) override;
    void Restore(Smp::String8 filename) override;
    void Reconnect(Smp::IComponent* root) override;
    void Exit() override;
    void Abort() override;
    Smp::SimulatorStateKind GetState() const override;
    void AddInitEntryPoint(Smp::IEntryPoint* entryPoint) override;
    void AddModel(Smp::IModel* model) override;
    void AddService(Smp::IService* service) override;
    Smp::IService* GetService(Smp::String8 name) const override;
    Smp::Services::ILogger* GetLogger() const override;
    Smp::Services::ITimeKeeper* GetTimeKeeper() const override;
    Smp::Services::IScheduler* GetScheduler() const override;
    Smp::Services::IEventManager* GetEventManager() const override;
    Smp::Services::IResolver* GetResolver() const override;
    Smp::Services::ILinkRegistry* GetLinkRegistry() const override;
    void RegisterFactory(Smp::IFactory* componentFactory) override;
    Smp::IComponent* CreateInstance(Smp::Uuid uuid, Smp::String8 name, Smp::String8 description,
                                    Smp::IComposite* parent) override;
    Smp::IFactory* GetFactory(Smp::Uuid uuid) const override;
    Smp::FactoryCollection* GetFactories() const override;
    Smp::Publication::ITypeRegistry* GetTypeRegistry() const override;
    void LoadLibrary(Smp::String8 libraryPath) override;
    /**
     * Create and add new model instance.
     * Model type is defined as template argument.
     * @param name new model instance name.
     * @param descr instance description.
     * @return pointer to the created model, may be nullptr in case
     * of creation error, for instance when object to create type does
     * not derive from Smp::IModel.
     */
    template <typename T>
    T* AddModel(Smp::String8 name, Smp::String8 descr = "") {
        T* m = new T(name, descr, this);
        auto mdl = dynamic_cast<Smp::IModel*>(m);
        if (mdl != nullptr) {
            AddModel(mdl);
        }
        else {
            delete m;
            m = nullptr;
        }
        return m;
    }

    /**
     * Connect 2 fields
     *
     * @param inputFieldPath
     * @param outputFieldPath
     */
    void connect(std::string inputFieldPath, std::string outputFieldPath);

    void schedule(std::string modelName, std::string entryPoint, uint32_t period);

    void setValue(std::string field, float value);

    Smp::IComponent* createSmpModel(Smp::String8 uuidStr, Smp::String8 name, Smp::String8 description);

    template <class T>
    T* find(Smp::String8 path) const {
        return dynamic_cast<T*>(GetResolver()->ResolveAbsolute(path));
    }

private:
    Smp::SimulatorStateKind _state;
    Collection<Smp::IEntryPoint> _initEntryPoints;
    Smp::IContainer* _models;
    Smp::IContainer* _services;
    Collection<Smp::IFactory> _compFactories;
    Smp::Services::ILogger* _logger;
    Scheduler* _scheduler;
    Smp::Services::ITimeKeeper* _timeKeeper;
    Smp::Services::IEventManager* _eventMgr;
    Smp::Services::ILinkRegistry* _linkRegistry;
    Smp::Publication::ITypeRegistry* _typeRegistry;
    Resolver* _resolver;
    std::vector<simph::sys::DLib*> _libs;

    void doPublish(Smp::IComponent* comp);
    void doConfigure(Smp::IComponent* comp);
    void doConnect(Smp::IComponent* comp);
    void setState(Smp::SimulatorStateKind newState);
    bool checkState(Smp::String8 opName, Smp::SimulatorStateKind expState);
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_Simulator_HPP__
