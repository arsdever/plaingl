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

    static std::shared_ptr<game_object> create_game_object()
    {
        auto object = std::shared_ptr<game_object>(new game_object);
        object->_id = instance()._registry.create();
        return object;
    }

    template <typename T, typename... ARGS>
        requires(std::is_base_of<components::component, T>::value)
    static T& create_component(game_object& obj, ARGS&&... args)
    {
        return instance()._registry.emplace<T>(
            obj.id(), std::forward<ARGS>(args)..., obj);
    }

    template <typename T>
        requires(std::is_base_of<components::component, T>::value)
    static T& get_component(const game_object& obj)
    {
        return instance()._registry.get<T>(obj.id());
    }

private:
    static memory_manager* _instance;
    entt::basic_registry<size_t> _registry;
    std::unordered_set<std::shared_ptr<game_object>> _objects;
};
