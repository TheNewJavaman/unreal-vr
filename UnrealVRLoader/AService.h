#pragma once

namespace UnrealVr {
    class AService {
    public:
        virtual void RegisterInjections() {}
        virtual ~AService() = default;
    };
}
