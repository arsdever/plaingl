#include <entt/entt.hpp>

#include "project/component.hpp"

#include "project/components/transform.hpp"
#include "project/game_object.hpp"

component::component(std::string_view name, game_object& obj)
    : _game_object(obj)
{
    set_name(std::string(name));
}

game_object& component::get_game_object() const { return _game_object; }

components::transform& component::get_transform() const
{
    return _game_object.get().get<components::transform>();
}

void component::set_active(bool active) { _is_active = active; }

bool component::is_active() const { return _is_active; }

void component::init()
{
    if (!is_active())
        return;
    on_init();
}

void component::update()
{
    if (!is_active())
        return;
    on_update();
}

void component::deinit()
{
    if (!is_active())
        return;
    on_deinit();
}

void component::on_init() { }

void component::on_update() { }

void component::on_deinit() { }
