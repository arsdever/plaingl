/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <glm/glm.hpp>

#include "camera.hpp"

#include "components/renderer_component.hpp"
#include "game_object.hpp"
#include "scene.hpp"

camera::camera() = default;

void camera::set_fov(float fov) { _fov = fov; }

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
        glm::radians(60.0f), _render_size.x / _render_size.y, 0.1f, 100.0f);
    glm::mat4 view =
        glm::lookAt(_transformation.get_position(), { 0, 0, 0 }, { 0, 1, 0 });

    return projection * view;
}

transform& camera::get_transform() { return _transformation; }

const transform& camera::get_transform() const { return _transformation; }

camera* camera::active_camera() { return camera::_active_camera; }

camera* camera::_active_camera = nullptr;
