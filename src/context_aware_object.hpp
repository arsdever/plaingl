#pragma once

#include <unordered_map>

template <typename T, typename K>
class context_aware_object
{
public:
    context_aware_object() = default;
    context_aware_object(const context_aware_object& other) = delete;
    context_aware_object(context_aware_object&& other) = default;
    context_aware_object& operator=(const context_aware_object& other) = delete;
    context_aware_object& operator=(context_aware_object&& other) = default;

    virtual ~context_aware_object() = 0;
    virtual bool activate() = 0;

protected:
    std::unordered_map<K, T> _context_dependent_map;
};

template <typename T, typename K>
context_aware_object<T, K>::~context_aware_object() = default;
