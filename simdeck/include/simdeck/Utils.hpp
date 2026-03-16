/*
 * @file Utils.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_Utils_HPP__
#define __simdeck_Utils_HPP__

#include <cstdint>
#include <cstddef>
#include <string>
#include <cxxabi.h>
#include "Smp/Uuid.h"
#include "Smp/ISimulator.h"
#include "simdeck/Collection.hpp"

#define UTILS_MD5_DIGEST_LENGTH 16

namespace simdeck {

/**
 * Misc utility functions set.
 */ 
struct Utils {
    /**
     * Generate an uuid for given model.
     * @param fullName the full name of the model (with factory prefix).
     */
    static Smp::Uuid GenerateUuid(const std::string& fullName);
    
    /**
     * Computes a buffer's MD5 hash.
     * @param pBuf input buffer.
     * @param bufSize input buffer size.
     * @param dest address of byte array receving the hash computation result.
     */
    static void CalcMD5(const void* pBuf, size_t bufSize, uint8_t dest[UTILS_MD5_DIGEST_LENGTH]);

    /**
     * Convert a buffer to an hex value string.
     * @param value pointer of buffer to convert.
     * @param size byte lenght of the buffer to convert.
     * @return the buffer content as an hex string.
     */
    static std::string Hex(uint8_t* value, size_t size);

    /**
     * Convert C++ symbbol name to clear type name.
     * @param name symbol name to convert.
     * @return converted name as string.
     */ 
    static std::string Demangle(std::string name) {
        int status = -1;
        char* dn = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status);
        if (status == 0) {
            name = dn;
            std::free(dn);
        }
        return name;
    }

    /**
     * Get an IObject full name.
     * full name is the full path (as usable with the resolver"s ResolveAbsolute
     * method) from the root object to the given object.
     * @param o object for which the full name is requested.
     * @return full name string.
     */
    static std::string GetFullName(const Smp::IObject* o);

    /**
     * Print into a logger the object tree.
     * @param from object to use as the root of the tree.
     * @param level indentation level (default 0, used internally with recursion)
     * @param sim host simulator.
     */
    static void LogDump(const Smp::IObject* from, int level=0, const Smp::ISimulator* sim=nullptr);


    /**
     * Print on an ostream (stdout by default) the object tree.
     * @param from object to use as the root of the tree.
     * @param level indentation level (default 0, used internally with recursion)
     * @param sim host simulator.
     * @param output the ostream used (stdout by default).
     */
    static void Dump(const Smp::IObject* from, int level=0, const Smp::ISimulator* sim=nullptr, std::ostream &output = std::cout);

    /**
     * Get an Object's children collection.
     * @param from object children seach object.
     * @return collection of all children of the object given by from argument.
     */ 
    static const Collection<Smp::IObject> GetChildren(const Smp::IObject* from);

    /**
     * search child by type
     * @param from object to search entity from.
     * @return 1st object found matching the template type. nullptr if none.
     */
    template <typename T>
    static T* GetChildOfType(Smp::IObject* from) {
        for (auto child: GetChildren(from)) {
            auto found=dynamic_cast<T*>(child);
            if (found!=nullptr) {
                return found;
            }
        }
        return nullptr;
    }
};

}  // namespace simdeck
#endif  // __simdeck_Utils_HPP__
