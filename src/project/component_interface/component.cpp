#include "project/component_interface/component.hpp"

#include "project/components/transform.hpp"
#include "project/game_object.hpp"
#include "project/project_manager.hpp"

component::component(std::string_view name, game_object& obj)
    : _game_object(obj)
{
    set_name(std::string(name));
}

game_object& component::get_game_object() const { return _game_object; }

components::transform& component::get_transform() const
{
    return static_cast<components::transform&>(
        _game_object.get().get("transform"));
}

void component::set_enabled(bool active) { _is_enabled = active; }

bool component::is_enabled() const { return _is_enabled; }

void component::init()
{
    if (!is_enabled())
        return;
    on_init();
}

void component::update()
{
    if (!is_enabled())
        return;
    on_update();
}

void component::deinit()
{
    if (!is_enabled())
        return;
    on_deinit();
}

size_t component::type_id(std::string_view class_name)
{
    return project_manager::type_id(class_name);
}

void component::on_init() { }

void component::on_update() { }

void component::on_deinit() { }
