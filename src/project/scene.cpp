#include <iostream>

#include "project/scene.hpp"

#include "project/game_object.hpp"
#include "project/memory_manager.hpp"

scene::scene() = default;

void scene::add_root_object(std::shared_ptr<game_object> object)
{
    _root_objects.push_back(object);
}

bool scene::visit_root_objects(
    std::function<bool(std::shared_ptr<game_object>&)> visitor)
{
    for (auto& object : _root_objects)
    {
        if (!visitor(object))
        {
            return false;
        }
    }
    return true;
}

bool scene::visit_root_objects(
    std::function<bool(const std::shared_ptr<game_object>&)> visitor) const
{
    for (auto& object : _root_objects)
    {
        if (!visitor(object))
        {
            return false;
        }
    }
    return true;
}

std::shared_ptr<scene> scene::create()
{
    return std::shared_ptr<scene>(new scene());
}

void scene::save(std::string_view path)
{
    std::ofstream file { std::string(path) };
    file << memory_manager::serialize().dump() << std::endl;
}

std::shared_ptr<scene> scene::load(std::string_view path)
{
    std::ifstream file { std::string(path) };
    nlohmann::json data;
    file >> data;
    _active_scene = std::shared_ptr<scene>(new scene());
    memory_manager::deserialize(data);
    return _active_scene;
}

std::shared_ptr<scene> scene::_active_scene { nullptr };
