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

    template <typename T, typename... ARGS>
        requires(std::is_base_of<components::component, T>::value)
    static T& create_component(game_object& obj, ARGS&&... args)
    {
        return instance()._registry.get_or_emplace<T>(
            entity(obj.id()), std::forward<ARGS>(args)..., obj);
    }

    template <typename T>
        requires(std::is_base_of<components::component, T>::value)
    static T& get_component(const game_object& obj)
    {
        return instance()._registry.get<T>(entity(obj.id()));
    }

    static nlohmann::json serialize();
    static void deserialize(const nlohmann::json& data);

    template <typename T>
    static void set_component_creator()
    {
        _component_creators[ entt::type_id<T>().hash() ] = [](game_object& obj)
        { create_component<T>(obj); };
    }

private:
    static std::shared_ptr<game_object> get_object(entt::entity ent);
    static entt::entity entity(uid id);
    static uid id(entt::entity ent);

private:
    static memory_manager* _instance;
    entt::registry _registry;
    std::unordered_map<uid, entt::entity> _objects;
    static std::unordered_map<entt::id_type, std::function<void(game_object&)>>
        _component_creators;
};
