#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <cstdint>
#include <string>
#include <vector>

namespace UnrealVR
{
    class PatternConversion
    {
    public:
        explicit PatternConversion(std::string pattern);

        std::vector<uint8_t> PatternBytes;
        std::vector<bool> PatternMask;

        bool IsMatch(const uint8_t* ptr) const;
    };

    class PatternStream : public std::vector<uint8_t*>
    {
    public:
        explicit PatternStream(const std::string& pattern);

        PatternStream HasPatternInRange(const std::string& pattern, int64_t offsetBegin = 0,
                                        int64_t offsetEnd = 64, bool replace = false) const;

        uint8_t* FirstOrNullptr() const;

        PatternStream& Log(const std::string& name);

    private:
        PatternStream() = default;
    };

    class ByteBuffer : public std::vector<uint8_t>
    {
    public:
        ByteBuffer(std::initializer_list<int32_t> l);
    };

    class PatternHelper
    {
    public:
        static void InitModuleInfo();
        static inline uint8_t* ModuleBegin = nullptr;
        static inline uint64_t ModuleSize = 0;

        static void InitProcessInfo();
        static inline HANDLE ProcessHandle = nullptr;
        
        static bool Write(uint8_t* ptr, const ByteBuffer& buffer);
    };
}
