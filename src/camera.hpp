#pragma once

#include <vector>

#include <glm/ext.hpp>

#include "transform.hpp"

class camera
{
public:
    camera();
    ~camera();

    void set_fov(float fov);
    camera* set_active();
    void set_render_size(float width, float height);

    void render();
    glm::mat4 vp_matrix() const;

    transform& get_transform();
    const transform& get_transform() const;

    static camera* active_camera();
    static const std::vector<camera*>& all_cameras();

private:
    transform _transformation;
    glm::vec2 _render_size;
    float _fov = 60.0f;
    static camera* _active_camera;
    static std::vector<camera*> _cameras;
};
