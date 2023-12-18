#include "components/sphere_collider_component.hpp"

#include "gizmo_drawer.hpp"
#include "logging.hpp"

namespace
{
inline logger log()
{
    return get_logger(sphere_collider_component::class_type_id);
}
} // namespace

sphere_collider_component::sphere_collider_component(game_object* parent)
    : collider_component(parent, class_type_id)
{
}

void sphere_collider_component::draw_gizmos()
{
    gizmo_drawer::instance()->draw_sphere(
        glm::vec3 { 0, 0, 0 }, _radius, { 0, 1, 0, 1 });
}

std::optional<collider_component::collision>
sphere_collider_component::detect_collision(std::array<glm::vec3, 2> ray)
{
    glm::vec3 obj_vec =
        get_game_object()->get_transform().get_position() - ray[ 0 ];
    auto distance = glm::length(glm::cross(obj_vec, ray[ 1 ]));
    log()->info("Distance: {}", distance);
    if (distance > _radius)
    {
        return std::nullopt;
    }

    return { { glm::vec3 {}, glm::vec3 {} } };
}
