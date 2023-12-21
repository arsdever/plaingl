#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "components/box_collider_component.hpp"

#include "gizmo_drawer.hpp"
#include "logging.hpp"

box_collider_component::box_collider_component(game_object* parent)
    : collider_component(parent, class_type_id)
{
}

glm::vec3 point;
glm::vec3 normal;
int index = 0;
void box_collider_component::draw_gizmos()
{
    glm::mat4 collider_to_object = glm::identity<glm::mat4>();
    collider_to_object = glm::translate(collider_to_object, get_position());
    collider_to_object *= glm::toMat4(get_rotation());
    collider_to_object =
        glm::scale(collider_to_object, { get_scale().x, get_scale().y, 1 });

    glm::mat4 object_to_world = get_game_object()->get_transform().get_matrix();

    glm::mat4 collider_to_world = collider_to_object * object_to_world;

    glm::vec3 global_scale;
    glm::quat global_rot;
    glm::vec3 global_pos;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(collider_to_world,
                   global_scale,
                   global_rot,
                   global_pos,
                   skew,
                   perspective);
    gizmo_drawer::instance()->draw_box(
        global_pos, global_rot, global_scale, { 0, 1, 0, 1 });
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
    auto [ center, right, up, forward ] = calculate_points_of_interest();

    std::optional<collider_component::collision> result = check_collision_plane(
        center + forward / 2.0f, right, up, ray[ 0 ], ray[ 1 ]);
    index = -1;

    if (!result.has_value())
    {
        result = check_collision_plane(
            center - forward / 2.0f, up, right, ray[ 0 ], ray[ 1 ]);
    }
    if (!result.has_value())
    {
        result = check_collision_plane(
            center - up / 2.0f, right, forward, ray[ 0 ], ray[ 1 ]);
    }
    if (!result.has_value())
    {
        result = check_collision_plane(
            center + up / 2.0f, forward, right, ray[ 0 ], ray[ 1 ]);
    }
    if (!result.has_value())
    {
        result = check_collision_plane(
            center + right / 2.0f, up, forward, ray[ 0 ], ray[ 1 ]);
    }
    if (!result.has_value())
    {
        result = check_collision_plane(
            center - right / 2.0f, forward, up, ray[ 0 ], ray[ 1 ]);
    }
    return result;
}

std::optional<collider_component::collision>
box_collider_component::check_collision_plane(glm::vec3 center,
                                              glm::vec3 right,
                                              glm::vec3 up,
                                              glm::vec3 ray_source,
                                              glm::vec3 ray_dir)
{
    glm::vec3 corner = center - right / 2.0f - up / 2.0f;
    glm::vec3 normal = glm::normalize(glm::cross(right, up));

    float dot_ray_normal = glm::dot(ray_dir, normal);

    if (dot_ray_normal == 0 || dot_ray_normal > 0)
    {
        return std::nullopt;
    }

    float hit_point_distance_from_ray =
        glm::dot((corner - ray_source), normal) / glm::dot(ray_dir, normal);
    // check if negative to determine if the collision happened behind the start
    // of the ray
    glm::vec3 hit_point_plane =
        ray_source + hit_point_distance_from_ray * ray_dir;

    glm::vec3 corner_to_hitpoint = hit_point_plane - corner;

    float proj_on_right =
        glm::dot(corner_to_hitpoint, right) / glm::length(right);
    float proj_on_up = glm::dot(corner_to_hitpoint, up) / glm::length(up);
    if (proj_on_right > 0 && proj_on_right < glm::length(right) &&
        proj_on_up > 0 && proj_on_up < glm::length(up))
    {
        return { { hit_point_plane, normal } };
    }

    return std::nullopt;
}

std::array<glm::vec3, 4> box_collider_component::calculate_points_of_interest()
{
    glm::mat4 collider_to_object = glm::identity<glm::mat4>();
    collider_to_object = glm::translate(collider_to_object, get_position());
    collider_to_object *= glm::toMat4(get_rotation());
    collider_to_object =
        glm::scale(collider_to_object, { get_scale().x, get_scale().y, 1 });

    glm::mat4 object_to_world = get_game_object()->get_transform().get_matrix();

    glm::mat4 collider_to_world = collider_to_object * object_to_world;

    glm::vec3 right = collider_to_world * glm::vec4 { 1, 0, 0, 1 };
    glm::vec3 up = collider_to_world * glm::vec4 { 0, 1, 0, 1 };
    glm::vec3 forward = collider_to_world * glm::vec4 { 0, 0, 1, 1 };
    glm::vec3 center = collider_to_world * glm::vec4 { 0, 0, 0, 1 };
    return { center, right, up, forward };
}
