#include <GLFW/glfw3.h>
#include <glm/gtx/quaternion.hpp>

#include "transform.hpp"

transform::transform()
    : _position({ 0, 0, 0 })
    , _rotation(glm::identity<glm::quat>())
    , _scale({ 1, 1, 1 })
{
}

void transform::set_position(glm::vec3 position) { _position = position; }

void transform::set_rotation(glm::quat rotation) { _rotation = rotation; }

void transform::set_scale(glm::vec3 scale) { _scale = scale; }

glm::vec3 transform::get_position() const { return _position; }

glm::quat transform::get_rotation() const { return _rotation; }

glm::vec3 transform::get_scale() const { return _scale; }

glm::mat4 transform::get_matrix() const
{
    glm::mat4 matrix = glm::identity<glm::mat4>();
    matrix = glm::translate(matrix, _position);
    matrix = matrix * glm::toMat4(_rotation);
    matrix = glm::scale(matrix, _scale);
    return matrix;
}
