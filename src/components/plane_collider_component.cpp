#include <optional>

#include <glm/gtx/matrix_decompose.hpp>

#include "components/plane_collider_component.hpp"

#include "components/text_component.hpp"
#include "gizmo_drawer.hpp"
#include "glm/gtx/quaternion.hpp"
#include "logging.hpp"

namespace
{
inline logger log()
{
    return get_logger(plane_collider_component::class_type_id);
}
} // namespace

plane_collider_component::plane_collider_component(game_object* parent)
    : collider_component(parent, class_type_id)
{
}

glm::vec3 point;
void plane_collider_component::draw_gizmos()
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

    gizmo_drawer::instance()->draw_plane(
        global_pos, global_rot, global_scale, { 0, 1, 0, 1 });
    gizmo_drawer::instance()->draw_grid(
        global_pos, global_rot, global_scale, 20, .1f);

    glm::vec3 normal = collider_to_world * glm::vec4 { 0, 0, 1, 1 };
    glm::vec3 right = collider_to_world * glm::vec4 { 1, 0, 0, 1 };
    glm::vec3 up = collider_to_world * glm::vec4 { 0, 1, 0, 1 };
    glm::vec3 center = collider_to_world * glm::vec4 { 0, 0, 0, 1 };
    glm::vec3 corner = center - right / 2.0f - up / 2.0f;

    gizmo_drawer::instance()->draw_ray(
        center, normal, .5f, glm::vec4 { 0, 0, 1, 1 });
    gizmo_drawer::instance()->draw_ray(
        point, normal, .5f, glm::vec4 { 1, 1, 0, 1 });
    gizmo_drawer::instance()->draw_ray(
        corner, right, 1.0f, glm::vec4 { 1, 0, 0, 1 });
    gizmo_drawer::instance()->draw_ray(
        corner, up, 1.0f, glm::vec4 { 0, 1, 0, 1 });
}

void plane_collider_component::set_position(glm::vec3 position)
{
    _position = position;
}

void plane_collider_component::set_rotation(glm::quat rotation)
{
    _rotation = rotation;
}

void plane_collider_component::set_scale(glm::vec2 scale) { _scale = scale; }

glm::vec3 plane_collider_component::get_position() const { return _position; }

glm::quat plane_collider_component::get_rotation() const { return _rotation; }

glm::vec2 plane_collider_component::get_scale() const { return _scale; }

std::optional<collider_component::collision>
plane_collider_component::detect_collision(std::array<glm::vec3, 2> ray)
{
    _text->set_text("not hit");
    auto [ normal, corner, right, up ] = params();

    float dot_ray_normal = glm::dot(ray[ 1 ], normal);

    if (dot_ray_normal == 0)
    {
        return std::nullopt;
    }

    float hit_point_distance_from_ray =
        glm::dot((corner - ray[ 0 ]), normal) / glm::dot(ray[ 1 ], normal);
    glm::vec3 hit_point_plane =
        ray[ 0 ] + hit_point_distance_from_ray * ray[ 1 ];

    glm::vec3 corner_to_hitpoint = hit_point_plane - corner;
    point = hit_point_plane;

    float proj_on_right =
        glm::dot(corner_to_hitpoint, right) / glm::length(right);
    float proj_on_up = glm::dot(corner_to_hitpoint, up) / glm::length(up);
    _text->set_text(fmt::format("{:16}({:#3.3} {:#3.3} {:#3.3})",
                                "!!! NOT HIT !!! ",
                                corner_to_hitpoint.x,
                                corner_to_hitpoint.y,
                                corner_to_hitpoint.z));

    if (proj_on_right > 0 && proj_on_right < glm::length(right) &&
        proj_on_up > 0 && proj_on_up < glm::length(up))
    {
        _text->set_text(fmt::format("{:16}({:#3.3} {:#3.3} {:#3.3})",
                                    "!!! HIT !!! ",
                                    corner_to_hitpoint.x,
                                    corner_to_hitpoint.y,
                                    corner_to_hitpoint.z));
        return { { hit_point_plane, normal } };
    }

    return std::nullopt;
}

std::array<glm::vec3, 4> plane_collider_component::params()
{
    glm::mat4 collider_to_object = glm::identity<glm::mat4>();
    collider_to_object = glm::translate(collider_to_object, get_position());
    collider_to_object *= glm::toMat4(get_rotation());
    collider_to_object =
        glm::scale(collider_to_object, { get_scale().x, get_scale().y, 1 });

    glm::mat4 object_to_world = get_game_object()->get_transform().get_matrix();

    glm::mat4 collider_to_world = collider_to_object * object_to_world;

    glm::vec3 normal = collider_to_world * glm::vec4 { 0, 0, 1, 1 };
    glm::vec3 right = collider_to_world * glm::vec4 { 1, 0, 0, 1 };
    glm::vec3 up = collider_to_world * glm::vec4 { 0, 1, 0, 1 };
    glm::vec3 center = collider_to_world * glm::vec4 { 0, 0, 0, 1 };
    glm::vec3 corner = center - right / 2.0f - up / 2.0f;
    return { normal, corner, right, up };
}
