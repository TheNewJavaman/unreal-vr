#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "AService.h"

#define REGISTER_SERVICE(T)                                  \
    {                                                        \
        auto t = std::make_shared<T>();                      \
        RegisterService(#T, t);                              \
        for (const auto& [ptr, name] : t->GetInjections()) { \
            RegisterInjection(name, ptr);                    \
        }                                                    \
    }
#define SERVICE(T, t) std::shared_ptr<T> (t) = nullptr;
#define INJECTION(T, ptr) { std::shared_ptr(&(ptr)), #T }
#define INJECT_SERVICE(T) InjectService(#T);
#define INJECT_SERVICE_AS(T, S) InjectServiceAs(#T, #S);
#define GET_SERVICE(T) GetService<T>(#T)

namespace UnrealVr {
    /** Services that can be injected into class members*/
    static inline std::map<std::string, std::shared_ptr<AService>> services;

    /** Class members that must be injected with services */
    static inline std::map<std::string, std::vector<std::shared_ptr<std::shared_ptr<AService>>>> injections;

    template<typename T>
    void RegisterService(const std::string& name, std::shared_ptr<T> service) {
        services.insert({ name, service });
    }

    template<typename T>
    std::shared_ptr<T> GetService(const std::string& name) {
        return std::reinterpret_pointer_cast<T>(services[name]);
    }

    template<typename T>
    void RegisterInjection(const std::string& name, std::shared_ptr<std::shared_ptr<T>> injection) {
        injections[name].push_back(std::shared_ptr(std::reinterpret_pointer_cast<AService>(injection.get())));
    }

    /**
     * Injects the specified service as another
     *
     * Useful for abstractions, such as injecting "D3D11Service" as "AGraphicsService"
     */ 
    void InjectServiceAs(const std::string& name, const std::string& as);

    void InjectService(const std::string& name);
}
