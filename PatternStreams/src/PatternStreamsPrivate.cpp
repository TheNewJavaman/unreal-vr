#include "PatternStreamsPrivate.h"

#include <Psapi.h>

namespace PS {
    BytePtrInterval ProcessInfo::GetModuleInterval(const std::string& module) {
        const auto found = ModuleIntervals.find(module);
        if (found == ModuleIntervals.end()) {
            const auto handle = GetModuleHandleA(module.empty() ? nullptr : module.c_str());
            BytePtrInterval interval = { reinterpret_cast<BytePtr>(handle) };
            MODULEINFO moduleInfo;
            GetModuleInformation(GetProcessHandle(), handle, &moduleInfo, sizeof moduleInfo);
            interval.Length = moduleInfo.SizeOfImage;
            ModuleIntervals.insert({ module, interval });
            return interval;
        }
        return found->second;
    }

    HANDLE ProcessInfo::GetProcessHandle() {
        if (ProcessHandle == nullptr) {
            ProcessHandle = GetCurrentProcess();
        }
        return ProcessHandle;
    }
}
