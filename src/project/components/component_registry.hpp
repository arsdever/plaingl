#pragma once

#define DECLARE_COMPONENT(c)                                                   \
    {                                                                          \
        components::c::type_name,                                              \
            []() -> size_t                                                     \
        {                                                                      \
            auto id = entt::hashed_string(components::c ::type_name.data(),    \
                                          components::c ::type_name.size());   \
            entt::meta<components::c>()                                        \
                .type(id)                                                      \
                .base<component>()                                             \
                .ctor<game_object&>()                                          \
                .func<&components::c ::serialize<json_serializer>>(            \
                    entt::hashed_string("serialize"))                          \
                .func<&components::c ::serialize<json_serializer>>(            \
                    entt::hashed_string("deserialize"));                       \
            memory_manager::register_component_type(components::c::type_name); \
            return id;                                                         \
        }                                                                      \
    }

class component_registry
{
public:
    static void register_components();

private:
    static std::unordered_map<std::string_view, std::function<size_t()>>
        _declaration_table;
};
