#include "components/sphere_collider_component.hpp"

#include "gizmo_drawer.hpp"

sphere_collider_component::sphere_collider_component(game_object* parent)
    : collider_component(parent, class_type_id)
{
}

void sphere_collider_component::draw_gizmos()
{
    gizmo_drawer::instance()->draw_sphere(
        glm::vec3 { 0, 0, 0 }, _radius, { 0, 1, 0, 1 });
}
