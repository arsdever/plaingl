#include "experimental/project/game_object.hpp"

#include "experimental/project/components/transform.hpp"
#include "experimental/project/memory_manager.hpp"

game_object::game_object() = default;

std::shared_ptr<game_object> game_object::create()
{
    auto obj = memory_manager::create_game_object();
    obj->add<components::transform>();
    obj->set_name("New game object");
    return obj;
}

components::transform& game_object::get_transform() const
{
    return get<components::transform>();
}

bool game_object::has_parent() const { return _parent.lock() != nullptr; }

game_object& game_object::get_parent() const { return *_parent.lock(); }

void game_object::set_parent(game_object& obj) { _parent = obj.shared_from_this(); }
