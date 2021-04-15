/*
 * @file Collection.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_Collection_HPP__
#define __simph_smpdk_Collection_HPP__
#include <string.h>
#include <algorithm>
#include <vector>
#include "Smp/ICollection.h"
#include "simph/smpdk/Object.hpp"

namespace simph {
namespace smpdk {

/**
 *
 */
template <typename T>
class Collection : public Object, public Smp::ICollection<T> {
public:
    /**
     * Default constructor.
     */
    Collection(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr)
        : Object(name, descr, parent) {}
    /**
     * Destructor.
     */
    virtual ~Collection() {}

    T* at(Smp::String8 name) const final {
        auto it = std::find_if(_content.begin(), _content.end(),
                               [name](const T* elem) { return elem != nullptr && strcmp(name, elem->GetName()) == 0; });
        return it != _content.end() ? *it : nullptr;
    }
    T* at(size_t index) const final {
        if (index < _content.size()) {
            return _content[index];
        }
        return nullptr;
    }
    size_t size() const final {
        return _content.size();
    }
    typedef typename Smp::ICollection<T>::const_iterator const_iterator;
    const_iterator begin() const final {
        return const_iterator(*this, 0);
    }
    const_iterator end() const final {
        return const_iterator(*this, _content.size());
    }
    bool contain(const T* t) const {
        return std::find(_content.begin(), _content.end(), t) != _content.end();
    }
    void push_back(T* t) {
        _content.push_back(t);
    }
    bool remove(const T* t) {
        size_t initialSize = _content.size();

        // This is better than the iterator shift loop O(n^2)
        // std::remove complexity is O(n). std::erase is O(1)
        _content.erase(std::remove(_content.begin(), _content.end(), t), _content.end());

        size_t newSize = _content.size();

        return initialSize != newSize;
    }

private:
    std::vector<T*> _content;
};

/*
 *
 */
template <typename T>
class CollectionOwner : public Collection<T> {
public:
    CollectionOwner(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent) : Collection<T>(name, descr, parent) {}

    virtual ~CollectionOwner() {
        for (auto field : *this) {
            delete field;
        }
    };
};

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_Collection_HPP__
