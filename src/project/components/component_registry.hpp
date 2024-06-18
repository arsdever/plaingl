#pragma once

class component_registry
{
public:
    template <typename T>
    static void register_component()
    {
        _registered_components.emplace(T::register_component());
    }

    template <typename... T>
    static void register_components()
    {
        (_registered_components.emplace(T::register_component()), ...);
    }

    static void for_each(std::function<void(size_t)> func)
    {
        for (auto id : _registered_components)
            func(id);
    }

private:
    static std::unordered_set<size_t> _registered_components;
};

inline std::unordered_set<size_t> component_registry::_registered_components;
