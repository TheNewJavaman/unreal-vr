#pragma once

#include <cstdint>

template<typename Target>
struct ACacheable {
    using TargetPtr = Target*;

    static inline TargetPtr target = nullptr;

    virtual bool RefreshCache() = 0;
};

template<typename Target, typename Parent>
struct ACacheableMember : public ACacheable<Target> {
    using ParentPtr = Parent*;

    ACacheableMember(ParentPtr parent) : parent(parent) {}

    ParentPtr parent;
};