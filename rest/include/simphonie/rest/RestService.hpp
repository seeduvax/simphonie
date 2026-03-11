/*
 * @file RestService.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_rest_RestService_HPP__
#define __simphonie_rest_RestService_HPP__
#include <condition_variable>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "Smp/IField.h"
#include "Smp/IPublication.h"
#include "Smp/ISimpleArrayField.h"
#include "Smp/ISimpleField.h"
#include "Smp/ISimulator.h"
#include "Smp/IStorageReader.h"
#include "Smp/IStorageWriter.h"
#include "Smp/Services/IResolver.h"
#include "Smp/Services/IScheduler.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simdeck/EntryPointPublisher.hpp"
#include "simdeck/Object.hpp"
#include "simdeck/Service.hpp"
#include "wfrest/HttpServer.h"
#include "wfrest/Json.h"

namespace simphonie {
namespace rest {

using namespace wfrest;

class RestService : public simdeck::Service,
                    public simdeck::EntryPointPublisher {
public:
    RestService(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    ~RestService();

private:
    class FieldHandler : public simdeck::Object,
                         public simdeck::EntryPointPublisher,
                         virtual public Smp::IStorageWriter,
                         virtual public Smp::IStorageReader {
    public:
        FieldHandler(Smp::IField* field, Smp::Services::IScheduler* scheduler, bool* simIsRunning, Smp::String8 name,
                     Smp::String8 descr, Smp::IObject* parent);

        std::vector<std::string> getValue();
        void setBinValue(const std::string& value);

        void Store(const Smp::Void* address, Smp::UInt64 size) override;
        void Restore(Smp::Void* address, Smp::UInt64 size) override;
        inline Smp::String8 GetStateVectorFileName() const override {
            return nullptr;
        };
        inline Smp::String8 GetStateVectorFilePath() const override {
            return nullptr;
        };

    private:
        void update();
        void scheduleUpdate();
        static inline uint8_t getHexPos(char c) {
            return (c <= '9') ? c - '0' : c - 'a' + 10;
        }

        Smp::Services::IScheduler* _scheduler;
        Smp::IEntryPoint* _updateEP;

        Smp::IField* _field;
        const Smp::ISimpleField* _simplefield;
        const Smp::ISimpleArrayField* _simplearrayfield;
        std::vector<Smp::AnySimple> _anysimples;
        std::string _bin;

        std::mutex _bufMutex;
        std::vector<char> _buf;

        std::mutex _updateMutex;
        bool _updated;
        bool* _simIsRunning;
        std::condition_variable _updatedCovar;
    };

    void publish(Smp::IPublication* reciever) override;
    void connect() override;
    void disconnect() override;
 
    static void setupResponse(HttpResp* resp);
    inline void onSimExecuting() {
        std::lock_guard<std::mutex> lock(_simIsRunningMutex);
        _simIsRunning = true;
    }
    inline void onSimLeavingExec() {
        std::lock_guard<std::mutex> lock(_simIsRunningMutex);
        _simIsRunning = false;
    }
    static std::string extractLastElemPath(std::string& path);
    bool removeSchedule(Smp::Services::EventId eventId);
    Json::Object parseTimestamp() const;
    static Json::Object parseType(const Smp::Publication::IType* type);
    template <typename T>
    static Json::Object parseKind(const T& kind);
    Json::Object parseField(Smp::IField* field);
    static Json::Object parseEP(const Smp::IEntryPoint* ep);
    Json::Array parseFields(const Smp::IComponent* Component);
    static Json::Array parseEPs(const Smp::IComponent* Component);
    Json::Object parseComponent(const Smp::IComponent* component, bool recursive);
    Json::Array parseContainer(const Smp::IContainer* container, bool recursive);
    void getSimulatorStatus(const HttpReq* req, HttpResp* resp);
    void getState(const HttpReq* req, HttpResp* resp) const;
    void postState(const HttpReq* req, HttpResp* resp);
    void postScheduleQueue(const HttpReq* req, HttpResp* resp);
    void defaultGetHandler(const HttpReq* req, HttpResp* resp);
    void defaultPostHandler(const HttpReq* req, HttpResp* resp);

    HttpServer _server;
    Smp::Int32 _family;
    std::string _host;
    Smp::UInt16 _port;
    Smp::ISimulator* _sim;
    Smp::Services::IResolver* _resolver;
    Smp::Services::ITimeKeeper* _timeKeeper;
    Smp::Services::IScheduler* _scheduler;
    std::mutex _simIsRunningMutex;
    bool _simIsRunning;
};

} /* namespace rest */
} /* namespace simphonie */
#endif /* __simphonie_rest_RestService_HPP__ */
