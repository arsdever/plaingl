#pragma once

#include <entt/entt.hpp>

#include "common/logging.hpp"
#include "project/project_manager.hpp"

template <typename T>
struct allocator
{
    static T* allocate(allocator<T>& a, size_t n) { }
};

class component_registry
{
public:
    static constexpr auto type_id(std::string_view type_name)
    {
        return entt::hashed_string(type_name.data(), type_name.size());
    }

    template <typename T>
    static constexpr auto type_id()
    {
        return entt::hashed_string(T ::type_name.data(), T ::type_name.size());
    }

    template <typename T, auto F>
    static T& type_constructor(entt::registry& reg,
                               std::string_view type_name,
                               entt::entity ent,
                               game_object& gobj)
    {
        auto& storage = reg.storage<T>(type_id(type_name));
        if (storage.contains(ent))
        {
            return storage.get(ent) = F(type_name, gobj);
        }
        else
        {
            storage.emplace(ent, gobj);
            return storage.get(ent) = F(type_name, gobj);
        }
    }

    template <typename T>
    static T& type_constructor(entt::registry& reg,
                               std::string_view type_name,
                               entt::entity ent,
                               game_object& gobj)
    {
        auto& storage = reg.storage<T>(type_id<T>());
        if (storage.contains(ent))
        {
            return storage.get(ent) = T(gobj);
        }
        else
        {
            return storage.emplace(ent, gobj);
        }
    }

    template <typename T>
    T* getter(entt::registry& reg, std::string_view type_name, entt::entity ent)
    {
        auto& storage = reg.storage<T>(type_id(type_name));
        if (storage.contains(ent))
        {
            return &storage.get(ent);
        }
        else
        {
            return nullptr;
        }
    }

    template <typename T, auto F = nullptr>
    static void register_component()
    {
        register_component<T, F>(T::type_name);
    }

    template <typename T, auto F = nullptr>
    static void register_component(std::string_view type_name)
    {
        auto id_of_type = type_id(type_name);
        get_logger("component_interface")
            ->debug("Registering component {} (id {}) in context {}",
                    type_name,
                    id_of_type.value(),
                    reinterpret_cast<long long>(&ctx));
        if constexpr (F == nullptr)
        {
            auto mf =
                entt::meta<T>(ctx)
                    .type(id_of_type)
                    .template ctor<&component_registry::type_constructor<T>,
                                   entt::as_ref_t>()
                    .template func<entt::overload<T*(entt::entity)>(
                        &entt::registry::try_get<T>)>(
                        entt::hashed_string("try_get"));
            //   .func<&T ::serialize<json_serializer>>(
            //       entt::hashed_string("serialize"));
            declare_base<typename T::base>(mf);
        }
        else
        {
            auto mf =
                entt::meta<T>(ctx)
                    .type(id_of_type)
                    .template ctor<&component_registry::type_constructor<T, F>,
                                   entt::as_ref_t>()
                    .template func<entt::overload<T*(entt::entity)>(
                        &entt::registry::try_get<T>)>(
                        entt::hashed_string("try_get"));
            declare_base<typename T::base>(mf);
        }
    }

    template <typename T>
    static void declare_base(auto& mf)
    {
        mf.template base<T>();
        if constexpr (requires { T::base; })
        {
            declare_base<T::base>(mf);
        }
    }

    template <typename... Ts>
    static void register_components()
    {
        (register_component<Ts>(), ...);
    }

    static entt::meta_ctx ctx;
};
