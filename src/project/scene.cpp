#include <iostream>

#include "project/scene.hpp"

#include "project/game_object.hpp"
#include "project/project_manager.hpp"

scene::scene() = default;

std::shared_ptr<scene> scene::get_active_scene() { return _active_scene; }

void scene::add_root_object(std::shared_ptr<game_object> object)
{
    _root_objects.push_back(object);
}

void scene::visit_root_objects(
    std::function<void(std::shared_ptr<game_object>&)> visitor)
{
    for (auto& object : _root_objects)
    {
        visitor(object);
    }
}

void scene::visit_root_objects(
    std::function<void(const std::shared_ptr<game_object>&)> visitor) const
{
    for (auto& object : _root_objects)
    {
        visitor(object);
    }
}

std::shared_ptr<scene> scene::create()
{
    return _active_scene = std::shared_ptr<scene>(new scene());
}

void scene::save(std::string_view path)
{
    std::ofstream file { std::string(path) };
    file << project_manager::serialize().dump() << std::endl;
}

std::shared_ptr<scene> scene::load(std::string_view path)
{
    std::ifstream file { std::string(path) };
    nlohmann::json data;
    file >> data;
    _active_scene = std::shared_ptr<scene>(new scene());
    project_manager::deserialize(data);
    return _active_scene;
}

std::shared_ptr<scene> scene::_active_scene { nullptr };
