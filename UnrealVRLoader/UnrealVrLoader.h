#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace UnrealVr {
    void OnAttach();
    DWORD __stdcall OnAttachThread(LPVOID);
    
    void OnDetach();
}