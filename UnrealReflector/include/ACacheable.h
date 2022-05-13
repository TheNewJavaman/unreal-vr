#pragma once

#include <cstdint>

template<typename Target>
class ACacheable {
public:
    using TargetPtr = Target*;

    static inline TargetPtr target = nullptr;

    virtual bool RefreshCache() = 0;
};

template<typename Target, typename Parent>
class ACacheableMember : public ACacheable<Target> {
public:
    using ParentPtr = Parent*;

    ACacheableMember(ParentPtr parent) : parent(parent) {}

    ParentPtr parent;
};