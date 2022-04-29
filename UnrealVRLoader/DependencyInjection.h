#pragma once

#include <map>
#include <ranges>
#include <string>
#include <vector>

#include "AService.h"

#define SERVICE(T)               \
    {                            \
        auto t = new T();        \
        RegisterService(#T, t);  \
        t->RegisterInjections(); \
    }
#define INJECTION(T, t) RegisterInjection(#T, &(t));
#define INJECT(T) InjectService<T>(#T);
#define GET_SERVICE(T) GetService<T>(#T)

namespace UnrealVr {
    static inline std::map<std::string, AService*> services;
    static inline std::map<std::string, std::vector<AService**>> injections;

    template<typename T>
    void RegisterService(const std::string& name, T* service) {
        services.insert({ name, service });
    }

    void DeleteServices();

    template<typename T>
    T* GetService(const std::string& name) {
        return reinterpret_cast<T*>(services[name]);
    }

    template<typename T>
    void RegisterInjection(const std::string& name, T** injection) {
        injections[name].push_back(injection);
    }

    template<typename T>
    void InjectService(const std::string& name) {
        for (AService** injection : injections[name]) {
            *injection = reinterpret_cast<T*>(injection);
        }
    }
}
