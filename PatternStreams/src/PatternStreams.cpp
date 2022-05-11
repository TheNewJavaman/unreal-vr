#include "PatternStreams.h"

#include "PatternStreamsPrivate.h"

namespace PS {
    bool Pattern::IsMatch(const BytePtr& ptr) const {
        for (size_t i = 0; i < size(); i++) {
            if (!at(i).isWildcard && this->at(i).value != *(ptr + i)) {
                return false;
            }
        }
        return true;
    }

    PatternStream::PatternStream(const Pattern& pattern, const std::string& module) {
        const auto interval = ProcessInfo::GetModuleInterval(module);
        for (BytePtr ptr = interval.Start; ptr < interval.Start + interval.Length; ptr++) {
            if (pattern.IsMatch(ptr)) {
                push_back(ptr);
            }
        }
    }

    PatternStream PatternStream::HasPatternInRange(
        const Pattern& pattern,
        const int64_t rangeOffset,
        const size_t rangeLength,
        const bool replaceExistingMatch
    ) const {
        PatternStream stream;
        for (const auto& ptr : *this) {
            for (size_t i = 0; i < rangeLength; i++) {
                if (pattern.IsMatch(ptr + rangeOffset + i)) {
                    if (replaceExistingMatch) {
                        stream.push_back(ptr + rangeOffset + i);
                    } else {
                        stream.push_back(ptr);
                    }
                }
            }
        }
        return stream;
    }

    PatternStream PatternStream::AddOffset(const int64_t offset) const {
        PatternStream stream = *this;
        for (auto& ptr : stream) {
            ptr += offset;
        }
        return stream;
    }

    PatternStream PatternStream::ForEach(const BytePtrFunc& func) const {
        PatternStream stream = *this;
        for (auto& ptr : stream) {
            func(ptr);
        }
        return stream;
    }

    PatternStream PatternStream::ForEach(const ConstBytePtrFunc& func) const {
        PatternStream stream = *this;
        for (const auto& ptr : stream) {
            func(ptr);
        }
        return stream;
    }

    BytePtr PatternStream::FirstOrNullPtr() const {
        return empty() ? nullptr : at(0);
    }
}
