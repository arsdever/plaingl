#include <glm/glm.hpp>

#include "camera.hpp"

camera::camera() = default;

void camera::set_position(glm::vec3 position)
{
    _position = std::move(position);
}

void camera::set_rotation(glm::quat rotation)
{
    _rotation = std::move(rotation);
}

void camera::set_fov(float fov) { _fov = fov; }
