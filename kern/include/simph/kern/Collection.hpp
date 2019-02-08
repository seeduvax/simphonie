/*
 * @file Collection.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Collection_HPP__
#define __simph_kern_Collection_HPP__
#include <string.h>
#include "Smp/ICollection.h"
#include "simph/kern/Object.hpp"
#include <vector>

namespace simph {
	namespace kern {

/**
 *
 */
template <typename T>
class Collection: public Object, public Smp::ICollection<T>  {
public:
    /**
     * Default constructor.
     */
    Collection(Smp::String8 name, Smp::String8 descr,
            Smp::IObject* parent): Object(name,descr,parent) {
    }
    /**
     * Destructor.
     */
    virtual ~Collection() {
    }

    T* at(Smp::String8 name) const {
        for (auto entry: _content) {
            if (strcmp(name,entry->GetName())==0) {
                return entry;
            }
        }
        return NULL;
    }
    T* at(size_t index) const {
        if (index<_content.size()) {
            return _content[index];
        }
        return NULL;
    }
    size_t size() const {
        return _content.size();
    }
    typedef typename Smp::ICollection<T>::const_iterator const_iterator;
    const_iterator begin() const {
        return const_iterator(*this,0);
    }
    const_iterator end() const {
        return const_iterator(*this,_content.size());
    }
    void push_back(T* t) {
        _content.push_back(t);
    }
private:
    std::vector<T*> _content;
};

}} // namespace simph::kern
#endif // __simph_kern_Collection_HPP__
