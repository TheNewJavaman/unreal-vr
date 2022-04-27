#pragma once

#include <map>
#include <string>

#define REGISTER_SERVICE(T) \
    { \
        T t = T(); \
        RegisterService(#T, t); \
    }
#define GET_SERVICE(T) \
    GetService(#T);

namespace UnrealVr {
    enum class Service;

    void RegisterServices();
    
    static inline std::map<std::string, Service&> registeredServices;

    template<typename T>
    void RegisterService(std::string name, T& service) {
        registeredServices.insert({ name, service });
    }

    template<typename T>
    T& GetService(const std::string name) {
        const auto match = registeredServices.find(name);
        if (match == registeredServices.end()) {
            return nullptr;
        }
        return match->second;
    }
}