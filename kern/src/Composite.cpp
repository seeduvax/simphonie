/*
 * @file Composite.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Composite.hpp"
#include "simph/kern/Container.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Composite::Composite(Smp::String8 name, Smp::String8 descr,
        Smp::IObject* parent): Object(name,descr,parent),
               _containers("Containers","",this) {
}
// ..........................................................
Composite::~Composite() {
    for (auto container: _containers) {
        delete container;
    }
}
// --------------------------------------------------------------------
// ..........................................................
void Composite::addContainer(Smp::String8 name,Smp::String8 descr) {
    _containers.push_back(new Container(name,descr,this)); 
}
// --------------------------------------------------------------------
// ..........................................................
const Smp::ContainerCollection* Composite::GetContainers() const {
    return &_containers;
}
// ..........................................................
Smp::IContainer* Composite::GetContainer(Smp::String8 name) const {
    return _containers.at(name);
}
// ..........................................................
void Composite::dump() {
    dump(std::cout,this);
}
// ..........................................................
void Composite::dump(std::ostream& out,Smp::IComposite* c,int level) {
    for (int i=0;i<level;++i) {
        out << "    ";
    }
    out << c->GetName()<<":" <</* TODO insert real type name here */ std::endl;
    for (auto container: *(c->GetContainers())) {
        for (int i=0;i<level;++i) {
            out << "    ";
        }
        out << "  " << container->GetName() << std::endl;
        for (auto comp: *(container->GetComponents())) {
            dump(out,comp,level+1);
        }
    }
}
// ..........................................................
void Composite::dump(std::ostream& out,Smp::IComponent* c,int level) {
    for (int i=0;i<level;++i) {
        out << "    ";
    }
    out << c->GetName()<<":" <</* TODO insert real type name here */ std::endl;
    for (auto field: *(c->GetFields())) {
        for (int i=0;i<level;++i) {
            out << "    ";
        }
        out << "    " << field->GetName() << /* TODO data type here*/ std::endl;
    }
}
}} // namespace simph::kern
