#pragma once

#include <nlohmann/json_fwd.hpp>

#include "project/uid.hpp"

class component;
class game_object;
class object;

class project_manager
{
public:
    ~project_manager();

    static void initialize();
    static void shutdown();

    static size_t type_id(std::string_view class_name);

    template <typename T>
    static size_t type_id()
    {
        return type_id(T::type_name);
    }

    static std::shared_ptr<game_object> create_game_object();

    static component& create_component(game_object& obj,
                                       std::string_view class_name);
    static component& get_component(const game_object& obj,
                                    std::string_view class_name);
    static std::shared_ptr<component>
    try_get_component(const game_object& obj, std::string_view class_name);

    static nlohmann::json serialize();
    static void deserialize(const nlohmann::json& data);

    template <typename T>
    static std::shared_ptr<T> try_get_object(uid id)
    {
        return std::dynamic_pointer_cast<T>(try_get_object(id));
    }

    template <typename T = object>
    static std::shared_ptr<T> get_object(uid id)
    {
        return std::static_pointer_cast<T>(try_get_object(id));
    }

    static std::shared_ptr<object> try_get_object(uid id);

    inline static std::shared_ptr<game_object> get_game_object(uid id)
    {
        return get_object<game_object>(id);
    }

    inline static std::shared_ptr<component> get_component(uid id)
    {
        return get_object<component>(id);
    }

    static bool
    visit_objects(std::function<bool(std::shared_ptr<object>)> visitor);
    static bool visit_game_objects(
        std::function<bool(std::shared_ptr<game_object>)> visitor);
    static bool
    visit_components(const game_object& obj,
                     std::function<bool(std::shared_ptr<component>)> visitor);

private:
    project_manager();

private:
    struct impl;
    std::unique_ptr<impl> _impl;
    static std::unique_ptr<project_manager> _instance;
};
