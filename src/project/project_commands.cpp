#include "project/project_commands.hpp"

#include "asset_management/asset_manager.hpp"
#include "common/logging.hpp"
#include "components/camera.hpp"
#include "components/light.hpp"
#include "components/mesh_filter.hpp"
#include "components/mesh_renderer.hpp"
#include "components/transform.hpp"
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
    _selected_object = _obj;
}

std::shared_ptr<game_object> cmd_create_game_object::get_object() const
{
    return _obj;
}

void cmd_load_scene::execute()
{
    if (get<0>() == "default")
    {
        auto s = default_scene();
        scene_loaded(s);
        return;
    }

    auto s = scene::load(get<0>());
    if (s != nullptr)
    {
        log()->info("Scene {} ({}) loaded", s->get_name(), s->id().id);
        scene_loaded(s);
    }
    else
    {
        log()->error("Failed to load scene {}", get<0>());
    }
}

std::shared_ptr<scene> cmd_load_scene::default_scene()
{
    auto s = scene::create();
    s->set_name("default");

    auto m = core::asset_manager::get<mesh>("cube");
    auto mat = core::asset_manager::get<graphics::material>("standard");

    auto go = game_object::create();
    go->add<components::mesh_filter>().set_mesh(m.get());
    go->add<components::mesh_renderer>().set_material(mat);
    go->set_name("suzanne");
    s->add_root_object(go);

    go = game_object::create();
    auto& l = go->add<components::light>();
    l.set_intensity(10.0);
    go->get_transform().set_position({ 3, 5, 3 });
    go->set_name("light_1");
    s->add_root_object(go);

    go = game_object::create();
    auto& camera = go->add<components::camera>();
    go->get_transform().set_position({ 10, 4, 8 });
    go->get_transform().set_rotation(
        glm::quatLookAt(glm::normalize(-go->get_transform().get_position()),
                        glm::dvec3 {
                            0.0,
                            1.0,
                            0.0,
                        }));
    camera.set_active();
    camera.set_fov(.6);
    camera.set_orthogonal(false);
    camera.set_background_color({ 0.1, 0.1, 0.1, 1.0 });
    go->set_name("camera");
    s->add_root_object(go);
    return s;
}

void cmd_save_scene::execute()
{
    if (auto s = scene::get_active_scene(); s != nullptr)
    {
        s->save(get<0>());
        log()->info("Scene {} ({}) saved", s->get_name(), s->id().id);
    }
    else
    {
        log()->error("No active scene");
    }
}

// cmd_destroy_game_object::cmd_destroy_game_object(
//     std::shared_ptr<game_object> obj)
//     : _obj(obj)
// {
// }

// void cmd_destroy_game_object::execute() { _obj->destroy(); }

void cmd_rename_object::execute()
{
    if (auto obj = _selected_object.lock(); obj != nullptr)
    {
        auto old_name = obj->get_name();
        obj->set_name(get<0>());
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

void cmd_select_object::execute()
{
    if (get<0>() == 0)
    {
        _selected_object = {};
        log()->info("Unselecting the object");
    }
    else
    {
        auto obj = memory_manager::get_object_by_id(uid(get<0>()));
        if (obj == nullptr)
        {
            log()->info("Object with id {} not found", get<0>());
            return;
        }

        _selected_object = obj;
        log()->info("Selected object {} ({})", obj->get_name(), obj->id().id);
    }
}

void cmd_print_selected_object::execute()
{
    if (auto obj = _selected_object.lock(); obj != nullptr)
    {
        log()->info("Selected object {} ({})", obj->get_name(), obj->id().id);
    }
}

void cmd_list_objects::execute()
{
    log()->info("List of objects:");
    memory_manager::instance().for_each_object([](std::shared_ptr<object>& obj)
    { log()->info("  {} ({})", obj->get_name(), obj->id().id); });
}

std::shared_ptr<object> selected_object() { return _selected_object.lock(); }
event<void(std::shared_ptr<scene>)> cmd_load_scene::scene_loaded;
} // namespace project
