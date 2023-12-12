/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <glm/glm.hpp>

#include "camera.hpp"

#include "components/renderer_component.hpp"
#include "game_object.hpp"
#include "scene.hpp"

camera::camera() { _cameras.push_back(this); }

camera::~camera() { std::erase(_cameras, this); }

void camera::set_fov(float fov) { _fov = fov; }

float camera::get_fov() const { return _fov; }

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
            if (obj->get_component<renderer_component>())
            {
                obj->get_component<renderer_component>()->render();
            }
        }
    }
    old_active_camera->set_active();
}

glm::mat4 camera::vp_matrix() const
{
    glm::mat4 projection = glm::perspective(
        glm::radians(_fov), _render_size.x / _render_size.y, 0.1f, 10000.0f);
    glm::vec3 direction =
        get_transform().get_rotation() * glm::vec3 { 0, 0, -1 };
    glm::vec3 cam_right =
        glm::normalize(glm::cross(direction, glm::vec3 { 0, 1, 0 }));
    glm::vec3 cam_up = glm::normalize(glm::cross(cam_right, direction));
    glm::vec3 cam_forward = glm::normalize(glm::cross(cam_right, cam_up));

    glm::mat3 view3(cam_right, cam_up, cam_forward);
    glm::mat4 view = view3;
    view = glm::inverse(glm::translate(glm::identity<glm::mat4>(),
                                       get_transform().get_position()) *
                        view);
    return projection * view;
}

transform& camera::get_transform() { return _transformation; }

const transform& camera::get_transform() const { return _transformation; }

camera* camera::active_camera() { return camera::_active_camera; }

const std::vector<camera*>& camera::all_cameras() { return camera::_cameras; }

camera* camera::_active_camera = nullptr;

std::vector<camera*> camera::_cameras;
