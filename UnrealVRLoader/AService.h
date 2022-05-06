#pragma once

#include <map>
#include <memory>
#include <string>

#include "ErrorHandling.h"

namespace UnrealVr {
    class AService;

    typedef std::map<std::shared_ptr<std::shared_ptr<AService>>, std::string> InjectionMap;

    /**
     * Defines required functionality for dependency injection
     */
    class AService {
    public:
        virtual InjectionMap GetInjections() { return {}; }
        virtual ErrorCode Init() { return ErrorCode::Success; }
        virtual ErrorCode Stop() { return ErrorCode::Success; }
        virtual ~AService() = default;
    };
}
