#pragma once

#include "ACacheable.h"

namespace UE {
    class UFunction;

    template<typename TEnum>
    class TEnumAsByte {
    public:
        uint8_t Value;
    };

    class UObject {
        public:
        class ProcessEvent_t : public ACacheableMember<void __cdecl(UObject*, UFunction*, void*), UObject> {
            public:
            ProcessEvent_t(UObject* parent) : ACacheableMember(parent) {}
            bool RefreshCache() override;
            void operator ()(UFunction* function, void* params);
        } ProcessEvent = { this };
    };

    class UFunction : public UObject {};
}