#pragma once
#include "Logging.h"

namespace UnrealVr::UE {
    class UFunction;

    class UObject {
    public:
        void ProcessEvent(UFunction* Function, void* Params);

    private:
        LOGGER(UObject)

        static typedef void (__cdecl ProcessEvent_t)(UObject*, UFunction* Function, void* Params);
        static inline ProcessEvent_t* ProcessEvent_Orig = nullptr;
    };

    class UFunction : public UObject {};
}
