/*
 * @file SmpIncrementC.h
 *
 * Copyright 2021. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */

/* Pour eviter des erreurs à la compile puisque simdeck pas à jour

#include "SmpIncrementC.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define Smp_SPACE CW_BUILD_SPACE(Smp)
#define Services_SPACE CW_BUILD_SPACE(Smp_SPACE, Services)
#define ILogger_SPACE CW_BUILD_SPACE(Services_SPACE, ILogger)
#define IEventManager_SPACE CW_BUILD_SPACE(Services_SPACE, IEventManager)
#define QueryLogMessageKind CW(ILogger_SPACE, QueryLogMessageKind)
#define LogMessageKind CW(Services_SPACE, LogMessageKind)
#define Log CW(ILogger_SPACE, Log)
#define QueryEventId CW(IEventManager_SPACE, QueryEventId)
#define Emit CW(IEventManager_SPACE, Emit)

#define EVENT_NAME "SmpIncrementEvent"

#ifdef __cplusplus
extern "C" {
#endif

struct env {
    void* evntMngr;
    void* logger;
    int64_t eventId;
    double input, output;
};

int publishSmpIncrementC(void* env, const SmpEnv* smpEnv) {
    struct env* theenv;

    theenv = (struct env*) env;

    CModelPublishField("input", "Input", &theenv->input, true, false);
    CModelPublishField("output", "Output", &theenv->output, false, true);

    return 0;
}

int configureSmpIncrementC(void* env, const SmpEnv* smpEnv) {
    return 0;
}

int connectSmpIncrementC(void* env, const SmpEnv* smpEnv) {
    struct env* theenv;

    theenv = (struct env*) env;
    theenv->evntMngr = CModelGetEventManager();
    theenv->eventId = QueryEventId(theenv->evntMngr, EVENT_NAME);
    theenv->logger = CModelGetLogger();

    return 0;
}

int freeSmpIncrementC(void* env, const SmpEnv* smpEnv) {
    free(env);
    return 0;
}

void SmpIncrementCStep(void* env, const SmpEnv* smpEnv) {
    struct env* theenv;

    theenv = (struct env*) env;

    const LogMessageKind kind = QueryLogMessageKind(theenv->logger, "Information");
    Log(theenv->logger, smpEnv->bridge, "calling SmpIncrementC's step", kind);

    theenv->output = theenv->input + 1;

    Emit(theenv->evntMngr, theenv->eventId, true);
}

int SmpIncrementC(CModel* model, const SmpEnv* smpEnv) {
    struct env* env;

    model->publish = &publishSmpIncrementC;
    model->configure = &configureSmpIncrementC;
    model->connect = &connectSmpIncrementC;
    model->free = &freeSmpIncrementC;
    model->env = malloc(sizeof(struct env));
    if (model->env == NULL) return 1;
    env = (struct env*) model->env;
    env->input = 0;
    env->output = 0;

    CModelAddEntryPoint("step", "Main model entry point", &SmpIncrementCStep);

    return 0;
}

#ifdef __cplusplus
}
#endif

*/
