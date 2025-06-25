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
#include <vector>
#include "Smp/IField.h"
#include "Smp/ISimpleField.h"
#include "Smp/ISimulator.h"
#include "Smp/IStorageReader.h"
#include "Smp/IStorageWriter.h"
#include "Smp/Services/IResolver.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simdeck/Service.hpp"
#include "simdeck/smpext/ISchedule.hpp"
#include "simdeck/smpext/ISchedulerObserver.hpp"
#include "wfrest/HttpServer.h"
#include "wfrest/Json.h"

namespace simphonie {
namespace rest {

using namespace wfrest;

class RestService : public simdeck::Service, virtual public simdeck::smpext::ISchedulerObserver {
public:
    RestService(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    ~RestService();

    void notifyScheduled(const simdeck::smpext::ISchedule* event) override;
    inline void notifyCompleted(Smp::Services::EventId eventId) override {
        removeSchedule(eventId);
    }
    inline void notifyCanceled(Smp::Services::EventId eventId) override {
        removeSchedule(eventId);
    }

private:
    class _FieldHandler : virtual public Smp::IStorageWriter, virtual public Smp::IStorageReader {
    public:
        inline _FieldHandler(Smp::IField* field)
            : _field(field), _simplefield(dynamic_cast<Smp::ISimpleField*>(_field)) {}

        void update();
        std::string getBinValue();
        inline bool isSimpleField() const {
            return _simplefield != nullptr;
        }
        inline std::string getStrValue() {
            std::ostringstream oss;
            oss << _anysimple;
            return oss.str();
        }
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
        static inline uint8_t getHexPos(char c) {
            return (c <= '9') ? c - '0' : c - 'a' + 10;
        }

        Smp::IField* _field;
        const Smp::ISimpleField* _simplefield;
        Smp::AnySimple _anysimple;
        std::string _binValue;
        std::vector<char> _buf;
        std::mutex _bufMtx;
        std::condition_variable _bufCovar;
    };

    struct _compareSchedule {
        bool operator()(const simdeck::smpext::ISchedule* a, const simdeck::smpext::ISchedule* b) const;
    };

    void connect();
    static std::string extractLastElemPath(std::string& path);
    bool removeSchedule(Smp::Services::EventId eventId);
    Json::Object parseTimestamp() const;
    static Json::Object parseType(const Smp::Publication::IType* type);
    template <typename T>
    static Json::Object parseKind(const T& kind);
    static Json::Object parseField(Smp::IField* field);
    static Json::Object parseEP(const Smp::IEntryPoint* ep);
    static Json::Object parseSchedule(const simdeck::smpext::ISchedule* schedule);
    static Json::Array parseFields(const Smp::IComponent* Component);
    static Json::Array parseEPs(const Smp::IComponent* Component);
    static Json::Object parseComponent(const Smp::IComponent* component, bool recursive);
    static Json::Array parseContainer(const Smp::IContainer* container, bool recursive);
    void getSimulator(const HttpReq* req, HttpResp* resp);
    void getState(const HttpReq* req, HttpResp* resp) const;
    void getScheduleQueue(const HttpReq* req, HttpResp* resp);
    void postState(const HttpReq* req, HttpResp* resp);
    void postScheduleQueue(const HttpReq* req, HttpResp* resp);
    void defaultGetHandler(const HttpReq* req, HttpResp* resp);
    void defaultPostHandler(const HttpReq* req, HttpResp* resp);

    HttpServer _server;
    Smp::ISimulator* _sim;
    Smp::Services::IResolver* _rslv;
    Smp::Services::ITimeKeeper* _tk;
    Smp::Services::IScheduler* _schdl;
    std::mutex _schdlMutex;
    std::multiset<const simdeck::smpext::ISchedule*, _compareSchedule> _scheduleQueue;
};

} /* namespace rest */
} /* namespace simphonie */
#endif /* __simphonie_rest_RestService_HPP__ */
