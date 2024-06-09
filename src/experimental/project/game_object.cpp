#include "experimental/project/game_object.hpp"

#include "experimental/project/memory_manager.hpp"

game_object::game_object() = default;

std::shared_ptr<game_object> game_object::create()
{
    return memory_manager::create<game_object>();
}
