#pragma once

#include <entt/entt.hpp>

#include "project/game_object.hpp"

class object;
class game_object;

namespace components
{
class component;
} // namespace components

class memory_manager
{
public:
    memory_manager();
    ~memory_manager();
    static void initialize();
    static void deinitialize();
    static memory_manager& instance();

    static entt::meta_type typeof(std::string_view class_name)
    {
        entt::hashed_string hs(class_name.data(), class_name.size());
        return entt::resolve(hs);
    }

    template <typename T>
    static entt::meta_type typeof()
    {
        return typeof(T::type_name);
    }

    static auto& storage_for(std::string_view class_name);

    static std::shared_ptr<game_object> get_object_by_id(uid id);

    static std::shared_ptr<game_object> create_game_object()
    {
        auto obj = std::shared_ptr<game_object>(new game_object);
        auto ent = instance()._registry.create();
        instance()._registry.emplace<std::shared_ptr<game_object>>(ent, obj);
        instance()._registry.emplace<uid>(ent, obj->id());
        instance()._objects.emplace(obj->id(), ent);
        return obj;
    }

    static components::component& create_component(game_object& obj,
                                                   std::string_view class_name);
    static components::component& get_component(const game_object& obj,
                                                std::string_view class_name);
    static components::component*
    try_get_component(const game_object& obj, std::string_view class_name);

    static bool
    visit_components(const game_object& obj,
                     std::function<bool(components::component&)> visitor);

    static nlohmann::json serialize();
    static void deserialize(const nlohmann::json& data);

    template <typename T>
    static void register_component_type()
    {
        instance()._registry.storage<T>(typeof<T>().id());
    }

private:
    static std::shared_ptr<game_object> get_object(entt::entity ent);
    static entt::entity entity(uid id);
    static uid id(entt::entity ent);

private:
    static memory_manager* _instance;
    entt::registry _registry;
    std::unordered_map<uid, entt::entity> _objects;
};
