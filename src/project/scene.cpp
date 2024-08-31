#include <nlohmann/json.hpp>

#include "project/scene.hpp"

#include "common/file.hpp"
#include "common/logging.hpp"
#include "project/game_object.hpp"
#include "project/memory_manager.hpp"

namespace
{
logger log() { return get_logger("project"); }
} // namespace

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
    file << memory_manager::serialize().dump() << std::endl;
}

std::shared_ptr<scene> scene::load(std::string_view path)
{
    if (!common::file::exists(path))
    {
        log()->error("Failed to load scene: {} does not exist", path);
        return nullptr;
    }

    try
    {
        nlohmann::json data =
            nlohmann::json::parse(common::file::read_all(path));
        _active_scene = std::shared_ptr<scene>(new scene());
        memory_manager::deserialize(data);
    }
    catch (nlohmann::json::exception& ex)
    {
        log()->error("Failed to load scene: {}", ex.what());
        return nullptr;
    }
    return _active_scene;
}

void scene::unload() { _active_scene = nullptr; }

std::shared_ptr<scene> scene::_active_scene { nullptr };
