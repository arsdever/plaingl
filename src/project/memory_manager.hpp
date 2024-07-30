#pragma once

#include <nlohmann/json_fwd.hpp>

#include "project/uid.hpp"

class component;
class game_object;
class object;

class memory_manager
{
public:
    memory_manager();
    ~memory_manager();

    static memory_manager& instance();

    static size_t type_id(std::string_view class_name);

    template <typename T>
    static size_t type_id()
    {
        return type_id(T::type_name);
    }

    static std::shared_ptr<object> get_object_by_id(uid id);

    template <typename T>
    static std::shared_ptr<T> get_object_by_id(uid id)
    {
        return std::dynamic_pointer_cast<T>(get_object_by_id(id));
    }

    static std::shared_ptr<game_object> create_game_object();

    static std::shared_ptr<game_object> get_game_object(uid id);

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

    static void
    for_each_object(std::function<void(std::shared_ptr<object>&)> func);

private:
    struct impl;
    std::unique_ptr<impl> _impl;
    std::unordered_map<uid, std::weak_ptr<object>> _objects;
};
