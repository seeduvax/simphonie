/*
 * @file Utils.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/Utils.hpp"
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include "simdeck/MD5.hpp"
#include "Smp/IEntryPointPublisher.h"
#include "Smp/IComposite.h"
#include "Smp/IArrayField.h"
#include "Smp/IStructureField.h"
#include "Smp/IOutputField.h"
#include "Smp/IModel.h"
#include "Smp/IService.h"
#include "Smp/Services/ILogger.h"
#include "Smp/Services/ILinkRegistry.h"
#include "Smp/Publication/IType.h"

namespace simdeck {
// ..........................................................
void Utils::CalcMD5(const void* pBuf, size_t bufSize, uint8_t dest[UTILS_MD5_DIGEST_LENGTH]) {
    MD5 md5;
    md5.update((const uint8_t*)pBuf, bufSize);
    md5.finalize();
    memcpy(dest,md5.getDigest(),UTILS_MD5_DIGEST_LENGTH);
}
// ..........................................................
std::string Utils::Hex(uint8_t* pBuf, size_t bufSize) {
    std::string ret;
    ret.reserve(bufSize * 2);
    const int HEX_LEN = 3;
    char hex[HEX_LEN];
    uint8_t* pCharBuf = (uint8_t*)pBuf;
    ::memset(hex, 0, HEX_LEN);
    for (uint16_t i = 0; i < bufSize; i++) {
        ::sprintf(hex, "%02x", pCharBuf[i]);
        ret += hex;
    }
    return ret;
}
// ..........................................................
Smp::Uuid Utils::GenerateUuid(const std::string& fullName) {
    uint8_t md5[UTILS_MD5_DIGEST_LENGTH];
    Utils::CalcMD5(fullName.c_str(), fullName.size(), md5);
    return Smp::Uuid(Utils::Hex(md5, UTILS_MD5_DIGEST_LENGTH).c_str());
}
// ..........................................................
std::string Utils::GetFullName(const Smp::IObject* o) {
    if (o == nullptr) {
        return "";
    }
    else {
        return GetFullName(o->GetParent()) + "/" + o->GetName();
    }
}

//...........................................................
void Utils::LogDump(const Smp::IObject* from, int level, const Smp::ISimulator* sim) {
    Smp::Services::ILogger* logger = nullptr;
    if (sim != nullptr) {
        logger = sim->GetLogger();
    }
    std::ostringstream output;
    Dump(from, level, sim, output);
    if (logger != nullptr) {
        logger->Log(sim, output.str().c_str(), Smp::Services::ILogger::LMK_Debug);
    } else {
        std::cout << "Warning: Cannot use ILogger from simulator" << std::endl;
        std::cout << output.str();
    }
}

// ..........................................................
void Utils::Dump(const Smp::IObject* from, int level, const Smp::ISimulator* sim, std::ostream &output)  {


    for (int i=0; i<level; i++) {
        output << "    ";
    }
    int l=level+1;
    output << from->GetName() << " ";
    if (dynamic_cast<const Smp::IService*>(from)!=nullptr) {
        output << "[Service]";
    }   
    if (dynamic_cast<const Smp::IModel*>(from)!=nullptr) {
        output << "[Model]";
    }   
    if (dynamic_cast<const Smp::IField*>(from)!=nullptr) {
        auto f=dynamic_cast<const Smp::IField*>(from);
        output << " [Field";
        if (f->IsInput()) {
            output << ":in";
        }
        if (f->IsOutput()) {
            output << ":out";
        }
        auto t = f->GetType();
        if (t != nullptr) {
            output << ":" << f->GetType()->GetPrimitiveTypeKind();
        }
        auto af = dynamic_cast<const Smp::IArrayField*>(from);
        if (af != nullptr) {
            output << "[" << af->GetSize() << "]";
        }
        output << "]";
        auto of=dynamic_cast<const Smp::IOutputField*>(f);
        if (of!=nullptr) {
            for (auto in: *(of->GetInputFields())) {
                output << std::endl;
                for (int i=0; i<level; i++) {
                    output << "    ";
                }
                output << "    -> " << GetFullName(in);
            }
        }
        auto sf=dynamic_cast<const Smp::IStructureField*>(from);
        if (sf!=nullptr) {
            for (auto f: *(sf->GetFields())) {
                Dump(f,l,sim);
            } 
        }
    }
    if (dynamic_cast<const Smp::ISimulator*>(from)!=nullptr) {
        output << "[Simulator]";
    }
    if (dynamic_cast<const Smp::IContainer*>(from)!=nullptr) {
        output << "[Container]";
    }
    if (dynamic_cast<const Smp::IComposite*>(from)!=nullptr) {
        output << "[Composite]";
    }
    if (dynamic_cast<const Smp::IEntryPoint*>(from)!=nullptr) {
        output << "[EntryPoint]";
    }
    output << std::endl;
    auto ctnr=dynamic_cast<const Smp::IContainer*>(from);
    if (ctnr!=nullptr) {
        for (auto cp: *(ctnr->GetComponents())) {
            Dump(cp,l,sim);
        }
    }
    auto cpst=dynamic_cast<const Smp::IComposite*>(from);
    if (cpst!=nullptr) {
        for (auto ct: *(cpst->GetContainers())) {
            Dump(ct,l,sim);
        }
    }
    auto epp=dynamic_cast<const Smp::IEntryPointPublisher*>(from);
    if (epp!=nullptr) {
        for (auto ep: *(epp->GetEntryPoints())) {
            Dump(ep,l,sim);
        } 
    }   
    auto c=dynamic_cast<const Smp::IComponent*>(from);
    if (c!=nullptr) {
        if (sim!=nullptr) {
            auto linkRegistry=sim->GetLinkRegistry();
            if (linkRegistry!=nullptr) {
                for (auto source : *(linkRegistry->GetLinkSources(c))) {
                    for (int i = 0; i < level; i++) {
                        output << "    ";
                    }
                    output << "    <-[" << linkRegistry->GetLinkCount(source, c) << "]- " << GetFullName(source)
                              << std::endl;
                }
            }
        }
        for (auto f: *(c->GetFields())) {
            Dump(f,l,sim);
        } 
    }
}
// ..........................................................
const Collection<Smp::IObject> Utils::GetChildren(const Smp::IObject* from) {
    Collection<Smp::IObject> children;
    auto af = dynamic_cast<const Smp::IArrayField*>(from);
    if (af != nullptr) {
        for (Smp::UInt64 i=0; i<af->GetSize(); i++) {
            children.push_back(af->GetItem(i));
        }
    }
    auto ctnr=dynamic_cast<const Smp::IContainer*>(from);
    if (ctnr!=nullptr) {
        for (auto cp: *(ctnr->GetComponents())) {
            children.push_back(cp);
        }
    }
    auto cpst=dynamic_cast<const Smp::IComposite*>(from);
    if (cpst!=nullptr) {
        for (auto ct: *(cpst->GetContainers())) {
            for (auto cp: *(ct->GetComponents())) {
                children.push_back(cp);
            }
        }
    }
    auto epp=dynamic_cast<const Smp::IEntryPointPublisher*>(from);
    if (epp!=nullptr) {
        for (auto ep: *(epp->GetEntryPoints())) {
            children.push_back(ep);
        } 
    }   
    auto c=dynamic_cast<const Smp::IComponent*>(from);
    if (c!=nullptr) {
        for (auto f: *(c->GetFields())) {
            children.push_back(f);
        } 
    }
    auto sf=dynamic_cast<const Smp::IStructureField*>(from);
    if (sf!=nullptr) {
        for (auto f: *(sf->GetFields())) {
            children.push_back(f);
        } 
    }
    return children;
}

}  // namespace simdeck
