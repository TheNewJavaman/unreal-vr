#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <map>

#include "PatternStreams.h"

namespace PS {
    struct BytePtrInterval {
        BytePtr Start;
        size_t Length;
    };

    class ProcessInfo {
    public:
        static BytePtrInterval GetModuleInterval(const std::string& module);
        static HANDLE GetProcessHandle();

    private:
        static inline std::map<std::string, BytePtrInterval> ModuleIntervals;
        static inline HANDLE ProcessHandle = nullptr;
    };
}
