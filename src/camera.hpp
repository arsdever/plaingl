#pragma once

#include <glm/ext.hpp>

#include "transform.hpp"

class camera
{
public:
    camera();

    void set_fov(float fov);
    void set_active();
    void set_render_size(float width, float height);

    void render();
    glm::mat4 vp_matrix() const;

    transform& get_transform();
    const transform& get_transform() const;

    static camera* active_camera();

private:
    transform _transformation;
    glm::vec2 _render_size;
    float _fov;
    static camera* _active_camera;
};
