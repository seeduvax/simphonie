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

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Composite::Composite() {
}
// ..........................................................
Composite::~Composite() {
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
    dump(std::cout);
}
// ..........................................................
void Composite::dump(std::ostream& out,int level) const {
    int lnext=level+1;
    for (int i=0;i<level;++i) {
	out << "    ";
    }
    out << GetName()<<":" <</* TODO insert real type name here */ std::endl;
    for (auto container: _containers) {
        for (int i=0;i<level;++i) {
            out << "    ";
        }
        out << "  " << container->GetName() << std::endl;
	for (auto comp: *(container->GetComponents())) {
            for (int i=0;i<lnext;++i) {
                out << "    ";
            }
            out << "  " << comp->GetName() << std::endl;
	}
    }
}
}} // namespace simph::kern
