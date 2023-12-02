#pragma once

#include <glm/ext.hpp>

class camera
{
public:
    camera();

    void set_position(glm::vec3 position);
    void set_rotation(glm::quat rotation);
    void set_fov(float fov);

    void render();

private:
    glm::vec3 _position;
    glm::quat _rotation;
    float _fov;
};
