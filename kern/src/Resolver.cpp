/*
 * @file Resolver.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/Resolver.hpp"

#include "Smp/IComponent.h"
#include "Smp/IComposite.h"
#include "Smp/IModel.h"
#include "Smp/IOutputField.h"
#include "Smp/IService.h"
#include "Smp/ISimpleArrayField.h"
#include "Smp/ISimpleField.h"
#include "Smp/ISimulator.h"
#include "Smp/Publication/IType.h"
#include "simphonie/kern/Publication.hpp"
#include "simphonie/sys/Callback.hpp"
#include "simphonie/sys/Logger.hpp"
#include "simdeck/Utils.hpp"

namespace simphonie {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Resolver::Resolver(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simdeck::Component(name, descr, parent) {
    // Default root is the parent object, until connect step where
    // the simulator is recovered.
    _root=parent;
    addEP("dump", "List published objects to stdout", this, &Resolver::dump);
}
// ..........................................................
Resolver::~Resolver() {
}
// --------------------------------------------------------------------
// ..........................................................
void Resolver::connect() {
    _root=getSimulator();
    _linkRegistry = getSimulator()->GetLinkRegistry();
}
// --------------------------------------------------------------------
// ..........................................................
Smp::IObject* Resolver::ResolveAbsolute(Smp::String8 absolutePath) {
    return resolveChild(absolutePath, _root);
}
// ..........................................................
Smp::IObject* Resolver::ResolveRelative(Smp::String8 relativePath, Smp::IObject* sender) {
    return resolveChild(relativePath, sender);
}
// --------------------------------------------------------------------
// ..........................................................
std::string Resolver::getFullName(Smp::IObject* o) const {
    if (o == _root || o == nullptr) {
        return "";
    }
    else {
        return getFullName(o->GetParent()) + "/" + o->GetName();
    }
}
// ..........................................................
void Resolver::dump() const {
    simdeck::Utils::Dump(_root, 0, dynamic_cast<const Smp::ISimulator*>(_root));
}

}  // namespace kern
}  // namespace simph
