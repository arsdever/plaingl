#pragma once

#include "common/utils.hpp"
#include "component.hpp"
#include "project/component_interface/metatype.hpp"
#include "project/project_manager.hpp"

class component_registry
{
public:
    static auto type_id(std::string_view type_name)
    {
        return std::hash<std::string_view>()(type_name);
    }

    template <typename T>
    static auto type_id()
    {
        return type_id(T::type_name);
    }

    static std::shared_ptr<component> instantiate(std::string_view type_name,
                                                  game_object& gobj)
    {
        auto it = _type_map.find(type_name);

        if (it == _type_map.end())
            return nullptr;

        return it->second(gobj);
    }

    template <typename... Ts>
    static void register_components()
    {
        (register_component<Ts>(), ...);
    }

    template <typename T>
    static void register_component()
    {
        register_component<T>(T::type_name);
    }

    template <typename T>
    static void register_component(std::string_view type_name)
    {
        if constexpr (requires { T::base; })
        {
            register_component<T::base>(type_name);
        }

        register_component(type_name,
                           [](game_object& obj)
        { return std::make_shared<T>(obj); });
    }

    static void register_component(
        std::string_view type_name,
        std::function<std::shared_ptr<component>(game_object&)> ctor)
    {
        _types.emplace(
            type_id(type_name),
            metatype { type_id(type_name), std::string(type_name), nullptr });
        _type_map[ std::string(type_name) ] = std::move(ctor);
    }

    template <typename T>
    static metatype get_type()
    {
        return _types.at(type_id(T::type_name));
    }

    static metatype get_type(std::string_view type_name)
    {
        return get_type(type_id(type_name));
    }

    static metatype get_type(size_t type_id)
    {
        if (auto it = _types.find(type_id); it != _types.end())
        {
            return it->second;
        }

        return {};
    }

    static std::unordered_map<
        std::string,
        std::function<std::shared_ptr<component>(game_object&)>,
        string_hash,
        std::equal_to<>>
        _type_map;

    static std::unordered_map<size_t, metatype> _types;
};
