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
        virtual ~AService() = default;
    };

    /**
     * Specifies that derived classes can be initialized
     */
    class AInitable {
    public:
        virtual ErrorCode Init() { return ErrorCode::Success; }
        virtual ~AInitable() = default;
    };
    
    /**
     * Specifies that derived classes can be stopped
     */
    class AStoppable {
    public:
        virtual ErrorCode Stop() { return ErrorCode::Success; }
        virtual ~AStoppable() = default;
    };
}
