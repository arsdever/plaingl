#pragma once

#include <memory>
#include <vector>

#include <glm/ext.hpp>

#include "transform.hpp"

class image;
class texture;
class mesh;
class shader_program;

class camera
{
public:
    camera();
    ~camera();

    void set_fov(float fov);
    float get_fov() const;
    void set_ortho(bool ortho_flag = true);
    float get_aspect_ratio() const;
    camera* set_active();
    void set_render_size(float width, float height);

    void set_background(glm::vec3 color);
    void set_background(image* img);

    void render();

    transform& get_transform();
    const transform& get_transform() const;

    static camera* active_camera();
    static const std::vector<camera*>& all_cameras();

    glm::mat4 projection_matrix() const;
    glm::mat4 view_matrix() const;
    glm::mat4 vp_matrix() const;

private:
    void draw_background();

private:
    transform _transformation;
    glm::vec2 _render_size;
    float _fov = 60.0f;
    bool _ortho_flag = false;
    static camera* _active_camera;
    static std::vector<camera*> _cameras;
    glm::vec3 _background_color;
    std::unique_ptr<texture> _background_texture = nullptr;
    std::unique_ptr<mesh> _background_quad = nullptr;
    std::unique_ptr<shader_program> _background_shader = nullptr;
};
