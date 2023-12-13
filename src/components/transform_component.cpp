#include <glm/gtx/quaternion.hpp>

#include "transform_component.hpp"

#include "gizmo_drawer.hpp"

transform_component::transform_component(game_object* parent)
    : component(parent, class_type_id)
{
}

void transform_component::draw_gizmos()
{
    gizmo_drawer::instance()->draw_line(
        { 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 0, 1 });
    gizmo_drawer::instance()->draw_line(
        { 0, 0, 0 }, { 0, 1, 0 }, { 0, 1, 0, 1 });
    gizmo_drawer::instance()->draw_line(
        { 0, 0, 0 }, { 0, 0, 1 }, { 0, 0, 1, 1 });
}

void transform_component::set_position(glm::vec3 position)
{
    get_game_object()->get_transform().set_position(std::move(position));
}

void transform_component::set_rotation(glm::quat rotation)
{
    get_game_object()->get_transform().set_rotation(std::move(rotation));
}

void transform_component::set_scale(glm::vec3 scale)
{
    get_game_object()->get_transform().set_scale(std::move(scale));
}

glm::vec3 transform_component::get_position() const
{
    return get_game_object()->get_transform().get_position();
}

glm::quat transform_component::get_rotation() const
{
    return get_game_object()->get_transform().get_rotation();
}

glm::vec3 transform_component::get_scale() const
{
    return get_game_object()->get_transform().get_scale();
}

glm::mat4 transform_component::get_matrix() const
{
    return get_game_object()->get_transform().get_matrix();
}
