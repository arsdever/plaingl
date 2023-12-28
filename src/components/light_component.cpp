#include "components/light_component.hpp"

#include "gizmo_drawer.hpp"
#include "light.hpp"

light_component::light_component(game_object* parent)
    : component(parent, class_type_id)
{
}

void light_component::set_light(light* l) { _light = l; }

light* light_component::get_light() { return _light; }

void light_component::update()
{
    _light->get_transform() = get_game_object()->get_transform();
}

void light_component::draw_gizmos()
{
    gizmo_drawer::instance()->draw_sphere(
        { 0, 0, 0 }, .1f, { 1.0f, 1.0f, 0.0f, 1.0f });
}
