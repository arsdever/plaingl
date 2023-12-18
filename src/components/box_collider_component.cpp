#include "components/box_collider_component.hpp"

#include "gizmo_drawer.hpp"
#include "logging.hpp"

namespace
{
inline logger log()
{
    return get_logger(box_collider_component::class_type_id);
}
} // namespace

box_collider_component::box_collider_component(game_object* parent)
    : collider_component(parent, class_type_id)
{
}

void box_collider_component::draw_gizmos()
{
    gizmo_drawer::instance()->draw_box(
        _position, _rotation, _scale, { 0, 1, 0, 1 });
}

void box_collider_component::set_position(glm::vec3 position)
{
    _position = position;
}

void box_collider_component::set_rotation(glm::quat rotation)
{
    _rotation = rotation;
}

void box_collider_component::set_scale(glm::vec3 scale) { _scale = scale; }

glm::vec3 box_collider_component::get_position() const { return _position; }

glm::quat box_collider_component::get_rotation() const { return _rotation; }

glm::vec3 box_collider_component::get_scale() const { return _scale; }

std::optional<collider_component::collision>
box_collider_component::detect_collision(std::array<glm::vec3, 2> ray)
{
    return std::nullopt;
}
