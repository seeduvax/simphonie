/*
 * @file Logger.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Logger.hpp"
#include <string.h>

namespace simph {
	namespace kern {
static Smp::String8 _LMK_NamesTable[]={
    Smp::Services::ILogger::LMK_InformationName,
    Smp::Services::ILogger::LMK_EventName,
    Smp::Services::ILogger::LMK_WarningName,
    Smp::Services::ILogger::LMK_ErrorName,
    Smp::Services::ILogger::LMK_DebugName
};

// --------------------------------------------------------------------
// ..........................................................
Logger::Logger() {
    setName("logger");
}
// ..........................................................
Logger::~Logger() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::Services::LogMessageKind Logger::QueryLogMessageKind(Smp::String8 messageKindName) {
    for (int i=0;i<=Smp::Services::ILogger::LMK_Debug;i++) {
        if (strcmp(messageKindName,_LMK_NamesTable[i])==0) {
            return i;
        }
    }
    return Smp::Services::ILogger::LMK_Debug;
}
// ..........................................................
void Logger::Log(const Smp::IObject* sender, Smp::String8 message,
                Smp::Services::LogMessageKind kind) {
    std::ostringstream s;
    s<<sender->GetName()<<"\t"<<message;
    _logger.log(_LMK_NamesTable[kind],s.str());
}


}} // namespace simph::kern
