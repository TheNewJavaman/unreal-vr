#pragma once

#include <functional>
#include <string>
#include <vector>

namespace PS {
    using Byte = uint8_t;
    using BytePtr = Byte*;
    using BytePtrFunc = std::function<void(BytePtr&)>;
    using ConstBytePtrFunc = std::function<void(const BytePtr&)>;

    class PatternByte {
    public:
        PatternByte() : value(0x00), isWildcard(true) {}
        PatternByte(const Byte value) : value(value), isWildcard(false) {}

        Byte value;
        bool isWildcard;
    };

    class Pattern : public std::vector<PatternByte> {
    public:
        Pattern(std::initializer_list<PatternByte> l) : std::vector<PatternByte>(l) {}

        bool IsMatch(const BytePtr& ptr) const;
    };

    class PatternStream : public std::vector<BytePtr> {
    public:
        PatternStream(const Pattern& pattern, const std::string& module = "");

        PatternStream HasPatternInRange(
            const Pattern& pattern,
            int64_t rangeOffset,
            size_t rangeLength,
            bool replaceExistingMatch = false
        ) const;

        PatternStream AddOffset(int64_t offset) const;

        PatternStream ForEach(const BytePtrFunc& func) const;

        PatternStream ForEach(const ConstBytePtrFunc& func) const;

        BytePtr FirstOrNullPtr() const;

    private:
        PatternStream() = default;
    };
}
