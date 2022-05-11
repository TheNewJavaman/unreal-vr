#pragma once

#include <cstdint>

template<typename Raw_t>
class ACacheable {
    public:
    typedef Raw_t* Target_t;
    Target_t target = nullptr;
    virtual bool RefreshCache() = 0;
};

template<typename Target_t, typename Parent_t>
class ACacheableMember : public ACacheable<Target_t> {
    public:
    ACacheableMember(Parent_t* parent) : parent(parent) {}
    Parent_t* parent;
};