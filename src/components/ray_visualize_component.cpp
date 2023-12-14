
#include "components/ray_visualize_component.hpp"

#include "gizmo_drawer.hpp"

ray_visualize_component::ray_visualize_component(game_object* parent)
    : component(parent, class_type_id)
{
}

void ray_visualize_component::set_ray(glm::vec3 from, glm::vec3 direction)
{
    _ray_from = from;
    _ray_direction = direction;
}

void ray_visualize_component::draw_gizmos()
{
    gizmo_drawer::instance()->draw_ray(
        _ray_from, _ray_direction, 10.0f, { 1, .5, .1, 1 });
}
