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
    float get_fov()const;
    void set_ortho(bool ortho_flag = true);
    float get_aspect_ratio() const;
    camera* set_active();
    void set_render_size(float width, float height);

    void render();
    glm::mat4 vp_matrix() const;
    // TODO: remove when merging
    void debug_vp_matrix();

    transform& get_transform();
    const transform& get_transform() const;

    static camera* active_camera();
    static const std::vector<camera*>& all_cameras();

private:
    transform _transformation;
    glm::vec2 _render_size;
    float _fov = 60.0f;
    bool _ortho_flag = false;
    static camera* _active_camera;
    static std::vector<camera*> _cameras;
    // TODO: remove when merging
    bool _debug_enabled = false;
};
