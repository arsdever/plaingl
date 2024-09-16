#include "project/component_interface/component.hpp"

#include "project/component_interface/component_registry.hpp"
#include "project/components/transform.hpp"
#include "project/game_object.hpp"
#include "project/project_manager.hpp"

component::component(std::string_view type_name, game_object& obj)
    : _game_object(obj)
{
    set_name(std::string(type_name));
    _type_info = component_registry::get_type(type_name);
}

game_object& component::get_game_object() const { return _game_object; }

std::shared_ptr<components::transform> component::get_transform() const
{
    return _game_object.get()
        .get_transform()
        .shared_from_this<components::transform>();
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

bool component::can_cast(const metatype& to_type) const
{
    return _type_info.can_cast(to_type);
}

bool component::can_cast(std::string_view type_name) const
{
    return can_cast(component_registry::get_type(type_name));
}

void component::on_init() { }

void component::on_update() { }

void component::on_deinit() { }
