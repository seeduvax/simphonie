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
#include "simph/kern/Composite.hpp"
#include "Smp/Publication/ITypeRegistry.h"
#include "simph/sys/DLib.hpp"

namespace simph {
	namespace kern {
class ObjectsRegistry;
class Scheduler;
/**
 *
 */
class Simulator: public virtual Composite, 
		public virtual Smp::ISimulator {
public:
    /**
     * Default constructor.
     */
    Simulator(Smp::String8 name="Simphonie",
            Smp::String8 descr="",
            Smp::IObject* parent=nullptr);
    /**
     * Destructor.
     */
    virtual ~Simulator();
    // Smp::ISimulator implementation
    void Initialise();
    void Publish();
    void Configure();
    void Connect();
    void Run();
    void Hold();
    void Store(Smp::String8 filename);
    void Restore(Smp::String8 filename);
    void Reconnect(Smp::IComponent* root);
    void Exit();
    void Abort();
    Smp::SimulatorStateKind GetState() const;
    void AddInitEntryPoint(Smp::IEntryPoint* entryPoint);
    void AddModel(Smp::IModel* model);
    void AddService(Smp::IService* service);
    Smp::IService* GetService(Smp::String8 name) const;
    Smp::Services::ILogger* GetLogger() const;
    Smp::Services::ITimeKeeper* GetTimeKeeper() const;
    Smp::Services::IScheduler* GetScheduler() const;
    Smp::Services::IEventManager* GetEventManager() const;
    Smp::Services::IResolver* GetResolver() const;
    Smp::Services::ILinkRegistry* GetLinkRegistry() const;
    void RegisterFactory(Smp::IFactory* componentFactory);
    Smp::IComponent* CreateInstance(Smp::Uuid uuid,
		       Smp::String8 name,
		       Smp::String8 description,
		       Smp::IComposite* parent);
    Smp::IFactory* GetFactory(Smp::Uuid uuid) const;
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
    T* AddModel(Smp::String8 name, Smp::String8 descr="") {
        T* m=new T(name,descr,_models);
        auto mdl=dynamic_cast<Smp::IModel*>(m);
        if (mdl!=nullptr) {
            AddModel(mdl);
        }
        else {
            delete m;
            m=nullptr;
        }
        return m;
    }
    /**
     * Dynamic load a library.
     * If library has an void Initialize(Smp::ISimulator*, Smp::ITypeRegistry*)
     * function, it is called just after loading. It should register bundled
     * type and component factories by calling bakc RegisterFactory and 
     * RegisterType.
     * TODO define behavior according to simulator state.
     * @param lib library name.
     */
    void loadLibrary(Smp::String8 lib);
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
    ObjectsRegistry* _registry;
    std::vector<simph::sys::DLib*> _libs;

    void publish(Smp::IComponent* comp);
    void configure(Smp::IComponent* comp);
    void connect(Smp::IComponent* comp);
    void setState(Smp::SimulatorStateKind newState);

};

}} // namespace simph::kern
#endif // __simph_kern_Simulator_HPP__
