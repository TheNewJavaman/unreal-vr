#pragma once

#include <map>
#include <memory>
#include <string>

#include "ErrorHandling.h"

namespace UnrealVr {
    class AService;

    typedef std::map<std::shared_ptr<std::shared_ptr<AService>>, std::string> InjectionMap;

    class AService {
    public:
        virtual InjectionMap GetInjections() { return {}; }
        virtual ~AService() = default;
    };

    class AInitable {
    public:
        virtual ErrorCode Init() { return ErrorCode::Success; }
        virtual ~AInitable() = default;
    };

    class AStoppable {
    public:
        virtual ErrorCode Stop() { return ErrorCode::Success; }
        virtual ~AStoppable() = default;
    };
}
