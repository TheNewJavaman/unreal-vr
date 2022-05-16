#pragma once

#include <functional>
#include <string>
#include <vector>

namespace PS {
    using Byte = uint8_t;
    using BytePtr = Byte*;
    using BytePtrFunc = std::function<void(BytePtr&)>;
    using ConstBytePtrFunc = std::function<void(const BytePtr&)>;

    struct PatternByte {
        PatternByte() : value(0x00), isWildcard(true) {}
        PatternByte(const Byte value) : value(value), isWildcard(false) {}

        Byte value;
        bool isWildcard;
    };

    struct Pattern : std::vector<PatternByte> {
        Pattern(std::initializer_list<PatternByte> l) : std::vector<PatternByte>(l) {}

        bool IsMatch(const BytePtr& ptr) const;
    };

    struct PatternStream : std::vector<BytePtr> {
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
