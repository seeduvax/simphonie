/*
 * @file CModel.h
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __CModel_H__
#define __CModel_H__
#include "CWrapper.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CModelAddEntryPoint(name, description, function)               \
    smpEnv->addEntryPoint(smpEnv->bridge, name, description, function)
#define CModelPublishField(name, description, pointer, input, output)               \
    smpEnv->publishField(smpEnv->bridge, name, description, pointer, input, output)
#define CModelGetComponent(name) smpEnv->getComponent(smpEnv->bridge, name)
#define CModelGetLogger() smpEnv->getLogger(smpEnv->bridge)
#define CModelGetTimeKeeper() smpEnv->getTimeKeeper(smpEnv->bridge)
#define CModelGetLinkRegistry() smpEnv->getLinkRegistry(smpEnv->bridge)
#define CModelGetScheduler() smpEnv->getScheduler(smpEnv->bridge)
#define CModelGetResolver() smpEnv->getResolver(smpEnv->bridge)
#define CModelGetEventManager() smpEnv->getEventManager(smpEnv->bridge)

typedef void Bridge;

struct SmpEnv;
typedef void (*CModelEP)(void* env, const struct SmpEnv* smpEnv);

#define CW_SPACE CW_BUILD_SPACE(Smp, Services)

typedef struct SmpEnv {
    int (*addEntryPoint)(Bridge* bridge, const char* name, const char* description, CModelEP ep);
    int (*publishField)(Bridge* bridge, const char* name, const char* description, void* pointer, bool input, bool output);
    CW(ILogger)* (*getLogger)(Bridge* bridge);
    // CW(ITimeKeeper)* (*getTimeKeeper)(Bridge* bridge); /* TODO */
    // CW(ILinkRegistry)* (*getLinkRegistry)(Bridge* bridge); /* TODO */
    // CW(IScheduler)* (*getScheduler)(Bridge* bridge); /* TODO */
    // CW(IResolver)* (*getResolver)(Bridge* bridge); /* TODO */
    CW(IEventManager)* (*getEventManager)(Bridge* bridge);
    void* bridge;
} SmpEnv;

#undef CW_SPACE

typedef struct CModel {
    int (*init)(void* env, const SmpEnv* smpEnv);
    int (*publish)(void* env, const SmpEnv* smpEnv);
    int (*configure)(void* env, const SmpEnv* smpEnv);
    int (*connect)(void* env, const SmpEnv* smpEnv);
    int (*free)(void* env, const SmpEnv* smpEnv);
    void* env;
} CModel;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CModel_H__ */
