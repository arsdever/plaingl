#pragma once

#include <glm/ext.hpp>

class camera
{
public:
    camera();

    void set_position(glm::vec3 position);
    void set_rotation(glm::quat rotation);
    void set_fov(float fov);
    void set_active();
    void set_render_size(float width, float height);

    void render();
    glm::mat4 vp_matrix() const;

    static camera* active_camera();

private:
    glm::vec3 _position;
    glm::quat _rotation;
    glm::vec2 _render_size;
    float _fov;
    static camera* _active_camera;
};
