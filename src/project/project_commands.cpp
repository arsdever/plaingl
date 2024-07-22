#include "project/project_commands.hpp"

#include "common/logging.hpp"
#include "memory_manager.hpp"
#include "project/game_object.hpp"
#include "project/scene.hpp"

namespace project
{
namespace
{
logger log() { return get_logger("project"); }
std::weak_ptr<object> _selected_object;
} // namespace

void cmd_create_game_object::execute()
{
    _obj = game_object::create();
    scene::get_active_scene()->add_root_object(_obj);
    log()->info("Game object {} ({}) was added to the scene ({})",
                _obj->get_name(),
                _obj->id().id,
                scene::get_active_scene()->id().id);
}

std::shared_ptr<game_object> cmd_create_game_object::get_object() const
{
    return _obj;
}

// cmd_destroy_game_object::cmd_destroy_game_object(
//     std::shared_ptr<game_object> obj)
//     : _obj(obj)
// {
// }

// void cmd_destroy_game_object::execute() { _obj->destroy(); }

cmd_rename_object::cmd_rename_object(std::string name)
    : _name(std::move(name))
{
}

void cmd_rename_object::execute()
{
    if (auto obj = _selected_object.lock(); obj != nullptr)
    {
        auto old_name = obj->get_name();
        obj->set_name(_name);
        log()->info("Object renamed ({}): {} -> {}",
                    obj->id().id,
                    old_name,
                    obj->get_name());
    }
    else
    {
        log()->info("No object selected");
    }
}

cmd_select_object::cmd_select_object(uid obj_id)
    : _object_id(obj_id)
{
}

void cmd_select_object::execute()
{
    auto obj = memory_manager::get_object_by_id(_object_id);
    _selected_object = obj;
    log()->info("Selected object {} ({})", obj->get_name(), obj->id().id);
}

void cmd_print_selected_object::execute()
{
    if (auto obj = _selected_object.lock(); obj != nullptr)
    {
        log()->info("Selected object {} ({})", obj->get_name(), obj->id().id);
    }
}
} // namespace project
