#include "experimental/project/scene.hpp"

#include "experimental/project/game_object.hpp"
#include "experimental/project/memory_manager.hpp"

scene::scene() = default;

void scene::add_object(std::shared_ptr<game_object> object)
{
    _objects.push_back(object);
}

std::shared_ptr<game_object> scene::create_object()
{
    auto object = memory_manager::create_game_object();
    add_object(object);
    return object;
}
