#pragma once

#define DECLARE_COMPONENT(c)                                                 \
    {                                                                        \
        components::c::type_name,                                            \
            []() -> size_t                                                   \
        {                                                                    \
            auto id = entt::hashed_string(components::c ::type_name.data(),  \
                                          components::c ::type_name.size()); \
            entt::meta<components::c>()                                      \
                .type(id)                                                    \
                .base<component>()                                           \
                .ctor<&entt::registry::emplace_or_replace<components::c,     \
                                                          game_object&>,     \
                      entt::as_ref_t>()                                      \
                .func<entt::overload<components::c*(entt::entity)>(          \
                    &entt::registry::try_get<components::c>)>(               \
                    entt::hashed_string("try_get"))                          \
                .func<&components::c ::serialize<json_serializer>>(          \
                    entt::hashed_string("serialize"));                       \
            project_manager::register_component_type(                        \
                components::c::type_name);                                   \
            return id;                                                       \
        }                                                                    \
    }

class component_registry
{
public:
    static void register_components();

private:
    static std::unordered_map<std::string_view, std::function<size_t()>>
        _declaration_table;
};
