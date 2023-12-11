#pragma once

#include <glm/ext.hpp>

class transform
{
public:
    transform();

    void set_position(glm::vec3 position);
    void set_rotation(glm::quat rotation);
    void set_scale(glm::vec3 scale);

    glm::vec3 get_position() const;
    glm::quat get_rotation() const;
    glm::vec3 get_scale() const;

    glm::mat4 get_matrix() const;

private:
    glm::vec3 _position;
    glm::quat _rotation;
    glm::vec3 _scale;
};
