/*
 * @file Callback.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_sys_Callback_HPP__
#define __simph_sys_Callback_HPP__

#include "simph/sys/Runnable.hpp"
#include <memory>

namespace simph {
    namespace sys {

class Callback {
public:
    using CallbackImplPtr = std::unique_ptr<Runnable>;

    template<typename _Callable, typename... _Args>
    static CallbackImplPtr create(_Callable&& func, _Args&&... args) {
        return createCallbackImpl(
                std::bind( std::forward<_Callable>(func),
                           std::forward<_Args>(args)... ));
    };

private:
    template<typename _Callable>
    struct CallbackImpl: public Runnable {
        _Callable _func;
        CallbackImpl(_Callable&& func):
            _func(std::forward<_Callable>(func)) {
        }
        virtual ~CallbackImpl() {
        }
        inline void run() final {
            _func();
        }
    };

    template<typename _Callable>
    static CallbackImplPtr createCallbackImpl(_Callable&& func) {
        using Impl = CallbackImpl<_Callable>;
        return CallbackImplPtr(new Impl{std::forward<_Callable>(func)});
    }
};

}} // namespace simph::sys
#endif // __simph_sys_Callback_HPP__
