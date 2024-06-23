#pragma once

#include <entt/entt.hpp>

#include "project/game_object.hpp"

class component;
class game_object;
class object;

class memory_manager
{
public:
    memory_manager();
    ~memory_manager();
    static void initialize();
    static void deinitialize();
    static memory_manager& instance();

    static size_t type_id(std::string_view class_name);

    template <typename T>
    static size_t type_id()
    {
        return type_id(T::type_name);
    }

    static entt::meta_type typeof(std::string_view class_name);

    template <typename T>
    static entt::meta_type typeof()
    {
        return typeof(T::type_name);
    }

    static auto& storage_for(std::string_view class_name);

    static std::shared_ptr<game_object> get_object_by_id(uid id);

    static std::shared_ptr<game_object> create_game_object();

    static component& create_component(game_object& obj,
                                       std::string_view class_name);
    static component& get_component(const game_object& obj,
                                    std::string_view class_name);
    static component* try_get_component(const game_object& obj,
                                        std::string_view class_name);

    static bool visit_components(const game_object& obj,
                                 std::function<bool(component&)> visitor);

    static nlohmann::json serialize();
    static void deserialize(const nlohmann::json& data);

    static void register_component_type(std::string_view type_name);

private:
    static std::shared_ptr<game_object> get_object(entt::entity ent);
    static entt::entity entity(uid id);
    static uid id(entt::entity ent);

private:
    static memory_manager* _instance;
    entt::registry _registry;
    std::unordered_map<uid, entt::entity> _objects;
};
