#include "PatternStream.h"

#include <Psapi.h>

#include "Utilities/Logger.h"

// From UML's Pattern.h
#define INRANGE(x,a,b)      (x >= a && x <= b)
#define getBits(x)          (INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte(x)          (getBits(x[0]) << 4 | getBits(x[1]))

namespace UnrealVR
{
    PatternConversion::PatternConversion(std::string pattern)
    {
        const auto numBytes = std::ranges::count(pattern, ' ') + 1;
        PatternBytes = std::vector<uint8_t>(numBytes);
        PatternMask = std::vector<bool>(numBytes);
        std::string temp;
        uint64_t charI = 0;
        for (int64_t byteI = 0; byteI < numBytes; charI++)
        {
            const auto patternChar = charI == pattern.size() ? ' ' : pattern.at(charI);
            if (patternChar == ' ')
            {
                if (temp == "?" || temp == "??")
                {
                    PatternMask.at(byteI) = false;
                }
                else
                {
                    PatternBytes.at(byteI) = getByte(temp);
                    PatternMask.at(byteI) = true;
                }
                temp = "";
                byteI++;
            }
            else
            {
                temp += patternChar;
            }
        }
    }

    bool PatternConversion::IsMatch(const uint8_t* ptr) const
    {
        for (uint64_t i = 0; i < PatternBytes.size(); i++)
        {
            if (PatternMask.at(i) && PatternBytes.at(i) != *(ptr + i))
            {
                return false;
            }
        }
        return true;
    }

    PatternStream::PatternStream(const std::string& pattern)
    {
        const PatternConversion conversion(pattern);
        PatternHelper::InitModuleInfo();
        for (uint8_t* i = PatternHelper::ModuleBegin; i < PatternHelper::ModuleBegin + PatternHelper::ModuleSize; i++)
        {
            if (conversion.IsMatch(i))
            {
                push_back(i);
            }
        }
    }

    PatternStream PatternStream::HasPatternInRange(const std::string& pattern, const int64_t offsetBegin,
                                                   const int64_t offsetEnd, const bool replace) const
    {
        PatternStream stream;
        const PatternConversion conversion(pattern);
        for (const auto& i : *this)
        {
            for (int64_t j = offsetBegin; j < offsetEnd; j++)
            {
                if (conversion.IsMatch(i + j))
                {
                    if (replace) stream.push_back(i + j);
                    else stream.push_back(i);
                    break;
                }
            }
        }
        return stream;
    }

    uint8_t* PatternStream::FirstOrNullptr() const
    {
        return empty() ? nullptr : at(0);
    }

    PatternStream& PatternStream::Log(const std::string& name)
    {
        Log::Info("[UnrealVR] Found %d pattern stream matches for %s:", size(), name.c_str());
        for (const auto& i : *this)
        {
            Log::Info("[UnrealVR] - 0x%p", i);
        }
        return *this;
    }

    ByteBuffer::ByteBuffer(const std::initializer_list<int32_t> l) : std::vector<uint8_t>(l.size())
    {
        for (size_t i = 0; i < l.size(); i++)
        {
            at(i) = static_cast<uint8_t>(*(l.begin() + i));
        }
    }

    void PatternHelper::InitModuleInfo()
    {
        if (ModuleBegin != nullptr) return;
        ModuleBegin = reinterpret_cast<uint8_t*>(GetModuleHandle(nullptr));
        MODULEINFO moduleInfo;
        GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(ModuleBegin),
                             &moduleInfo, sizeof moduleInfo);
        ModuleSize = moduleInfo.SizeOfImage;
    }

    void PatternHelper::InitProcessInfo()
    {
        if (ProcessHandle != nullptr) return;
        ProcessHandle = GetCurrentProcess();
    }

    bool PatternHelper::Write(uint8_t* ptr, const ByteBuffer& buffer)
    {
        InitProcessInfo();
        return WriteProcessMemory(ProcessHandle, ptr, buffer.data(), buffer.size(), nullptr);
    }
}
