#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "UnrealVrService.h"

BOOL APIENTRY DllMain(HMODULE, const DWORD ul_reason_for_call, LPVOID) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        UnrealVr::OnAttach();
        break;
    case DLL_PROCESS_DETACH:
        UnrealVr::OnDetach();
        break;
    default:
        break;
    }
    return TRUE;
}
