#include "DependencyInjection.h"

namespace UnrealVr {
    void InjectServiceAs(const std::string& name, const std::string& as)  {
        for (const auto& injection : injections[as]) {
            *injection = services[name];
        }
    }

    void InjectService(const std::string& name)  {
        InjectServiceAs(name, name);
    }
}