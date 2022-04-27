#include "UnrealVrLoader.h"

#include "DependencyInjection.h"

namespace UnrealVr {
    void OnAttach() {
        CreateThread(nullptr, 0, OnAttachThread, nullptr, 0, nullptr);
    }

    DWORD OnAttachThread(LPVOID) {
        RegisterServices();
        return 0;
    }
}