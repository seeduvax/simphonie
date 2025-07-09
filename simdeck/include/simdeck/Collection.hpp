/*
 * @file Collection.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_Collection_HPP__
#define __simdeck_Collection_HPP__
#include <cstring>
#include <algorithm>
#include <vector>
#include "simdeck/Object.hpp"
#include "Smp/ICollection.h"

#include <iostream>

namespace simdeck {

/**
 * SMP::ICollection implementation.
 */
template <typename T>
class Collection : public Smp::ICollection<T> {
public:
    /**
     * Default constructor.
     */
    Collection() {}
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
    Smp::Bool empty() const override {
        return _content.empty();
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

    void clear() { _content.clear(); }
    bool empty() { return _content.empty(); }
    
    Collection<T>& operator=(const Smp::ICollection<T>& other) {
        _content.clear();
        for (T* element : other) {
            _content.push_back(element);
        }
        return *this;
    }

private:
    std::vector<T*> _content;
};

/*
 *
 */
template <typename T>
class OwnedCollection : public Collection<T> {
public:
    OwnedCollection() {}

    virtual ~OwnedCollection() {
        for (auto field : *this) {
            delete field;
        }
    };
};

}  // namespace simdeck
#endif  // __simdeck_Collection_HPP__
