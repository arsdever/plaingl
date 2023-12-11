#include <glm/glm.hpp>

#include "camera.hpp"

camera::camera() = default;

void camera::set_fov(float fov) { _fov = fov; }

void camera::set_active() { _active_camera = this; }

void camera::set_render_size(float width, float height)
{
    _render_size = { width, height };
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
