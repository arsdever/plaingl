#pragma once

#include <memory>
#include <vector>

#include <glm/ext.hpp>

#include "transform.hpp"

class framebuffer;
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
    void set_render_size(size_t width, size_t height);
    void set_render_size(glm::uvec2 size);
    void set_render_texture(std::weak_ptr<texture> render_texture);
    std::shared_ptr<texture> get_render_texture() const;
    void set_gizmos_enabled(bool flag = true);
    bool get_gizmos_enabled() const;

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
    void render_on_private_texture() const;
    void render_gizmos() const;
    void setup_lights();

private:
    transform _transformation;
    glm::uvec2 _render_size { 1, 1 };
    float _fov = 60.0f;
    bool _ortho_flag = false;
    std::weak_ptr<texture> _user_render_texture {};
    glm::vec3 _background_color { 0.0f, 0.0f, 0.0f };
    std::unique_ptr<texture> _background_texture = nullptr;
    std::unique_ptr<mesh> _background_quad = nullptr;
    std::unique_ptr<shader_program> _background_shader = nullptr;
    unsigned _lights_buffer = 0;
    bool _gizmos_enabled = false;
    std::unique_ptr<framebuffer> _framebuffer { nullptr };

    static camera* _active_camera;
    static std::vector<camera*> _cameras;
};
