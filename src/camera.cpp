/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <glm/glm.hpp>

#include "camera.hpp"

#include "components/renderer_component.hpp"
#include "game_object.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "scene.hpp"

namespace
{
static logger log() { return get_logger("camera"); }
} // namespace

camera::camera() { _cameras.push_back(this); }

camera::~camera() { std::erase(_cameras, this); }

void camera::set_fov(float fov) { _fov = fov; }

float camera::get_fov() const { return _fov; }

void camera::set_ortho(bool ortho_flag) { _ortho_flag = ortho_flag; }

float camera::get_aspect_ratio() const
{
    return _render_size.x / _render_size.y;
}

camera* camera::set_active()
{
    auto* old = _active_camera;
    _active_camera = this;
    return old;
}

void camera::set_render_size(float width, float height)
{
    _render_size = { width, height };
}

void camera::render()
{
    auto* old_active_camera = set_active();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (scene::get_active_scene())
    {
        for (auto* obj : scene::get_active_scene()->objects())
        {
            if (!obj->is_active())
            {
                continue;
            }
            if (auto* renderer = obj->get_component<renderer_component>();
                renderer)
            {
                renderer->get_material()->set_property_value(
                    "model_matrix", obj->get_transform().get_matrix());
                renderer->render();
            }
        }
    }
    old_active_camera->set_active();
}

glm::mat4 camera::vp_matrix() const
{
    return projection_matrix() * view_matrix();
}

glm::mat4 camera::view_matrix() const
{
    // TODO: optimize with caching
    glm::quat rotation = get_transform().get_rotation();
    glm::vec3 direction = rotation * glm::vec3 { 0, 0, 1 };
    glm::vec3 cam_right =
        glm::normalize(glm::cross(direction, glm::vec3 { 0, 1, 0 }));
    glm::vec3 cam_up = glm::normalize(glm::cross(cam_right, direction));
    glm::vec3 cam_forward = glm::normalize(glm::cross(cam_right, cam_up));

    glm::mat3 view3(cam_right, cam_up, cam_forward);
    glm::mat4 view = view3;
    view = glm::inverse(glm::translate(glm::identity<glm::mat4>(),
                                       get_transform().get_position()) *
                        view);

    return view;
}

glm::mat4 camera::projection_matrix() const
{
    // TODO: optimize with caching
    if (_ortho_flag)
    {
        glm::quat rotation = get_transform().get_rotation();
        glm::vec3 direction = rotation * glm::vec3 { 0, 0, 1 };
        float dist = glm::dot(direction, get_transform().get_position());

        return glm::ortho(
            -_render_size.x /
                glm::distance(get_transform().get_position(), { 0, 0, 0 }),
            _render_size.x /
                glm::distance(get_transform().get_position(), { 0, 0, 0 }),
            -_render_size.y /
                glm::distance(get_transform().get_position(), { 0, 0, 0 }),
            _render_size.y /
                glm::distance(get_transform().get_position(), { 0, 0, 0 }),
            0.01f,
            10000.0f);
    }

    return glm::perspective(
        glm::radians(_fov), _render_size.x / _render_size.y, 0.01f, 10000.0f);
}

transform& camera::get_transform() { return _transformation; }

const transform& camera::get_transform() const { return _transformation; }

camera* camera::active_camera() { return camera::_active_camera; }

const std::vector<camera*>& camera::all_cameras() { return camera::_cameras; }

camera* camera::_active_camera = nullptr;

std::vector<camera*> camera::_cameras;
