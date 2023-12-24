/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "camera.hpp"

#include "components/renderer_component.hpp"
#include "game_object.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace
{
static logger log() { return get_logger("camera"); }
} // namespace

camera::camera()
{
    _cameras.push_back(this);
    _background_quad = std::make_unique<mesh>();
    std::array<glm::vec2, 4> verts {
        { { -1, -1 }, { -1, 1 }, { 1, 1 }, { 1, -1 } }
    };

    std::vector<vertex3d> vertices;
    for (auto& v : verts)
    {
        vertex3d single_vertex;
        single_vertex.position() = { v.x, v.y, 0 };
        single_vertex.uv() = { v.x, v.y };
        vertices.push_back(std::move(single_vertex));
    }

    _background_quad->set_vertices(std::move(vertices));
    _background_quad->set_indices({ 0, 1, 2, 0, 2, 3 });
    _background_quad->init();

    _background_shader = std::make_unique<shader_program>();
    _background_shader->init();
    _background_shader->add_shader("camera_background_shader.vert");
    _background_shader->add_shader("camera_background_shader.frag");
    _background_shader->link();
}

camera::~camera() { std::erase(_cameras, this); }

void camera::set_fov(float fov) { _fov = fov; }

float camera::get_fov() const { return _fov; }

void camera::set_ortho(bool ortho_flag) { _ortho_flag = ortho_flag; }

float camera::get_aspect_ratio() const
{
    return _render_size.x / _render_size.y;
}

camera* camera::set_active()
{
    auto* old = _active_camera;
    _active_camera = this;
    return old;
}

void camera::set_render_size(float width, float height)
{
    _render_size = { width, height };
}

void camera::set_background(glm::vec3 color)
{
    _background_color = color;
    _background_shader->set_uniform("background_color",
                                    std::make_tuple(color.r, color.g, color.b));
    _background_shader->set_uniform("use_color", std::make_tuple(1.0f));
}

void camera::set_background(image* img)
{
    if (!img)
    {
        _background_texture.reset();
    }
    else
    {
        _background_texture =
            std::make_unique<texture>(std::move(texture::from_image(img)));
        _background_shader->set_uniform(
            "cubemap", std::make_tuple(_background_texture.get()));
        _background_shader->set_uniform("use_color", std::make_tuple(0.0f));
    }
}

void camera::render()
{
    auto* old_active_camera = set_active();

    _background_shader->set_uniform(
        "camera_matrix",
        std::make_tuple(glm::toMat4(get_transform().get_rotation()) *
                        glm::inverse(projection_matrix())));
    if (_background_texture)
    {
        _background_texture->bind(0);
    }

    _background_shader->use();
    _background_quad->render();
    shader_program::unuse();

    glEnable(GL_DEPTH_TEST);

    if (scene::get_active_scene())
    {
        for (auto* obj : scene::get_active_scene()->objects())
        {
            if (!obj->is_active())
            {
                continue;
            }
            if (auto* renderer = obj->get_component<renderer_component>();
                renderer)
            {
                renderer->get_material()->set_property_value(
                    "model_matrix", obj->get_transform().get_matrix());
                renderer->render();
            }
        }
    }

    if (old_active_camera)
    {
        old_active_camera->set_active();
    }
}

glm::mat4 camera::vp_matrix() const
{
    return projection_matrix() * view_matrix();
}

glm::mat4 camera::view_matrix() const
{
    // TODO: optimize with caching
    glm::quat rotation = get_transform().get_rotation();
    glm::vec3 cam_right = rotation * glm::vec3 { -1, 0, 0 };
    glm::vec3 cam_up = rotation * glm::vec3 { 0, 1, 0 };
    glm::vec3 cam_forward = rotation * glm::vec3 { 0, 0, -1 };

    glm::mat3 view3(cam_right, cam_up, cam_forward);
    glm::mat4 view = view3;
    view = glm::inverse(glm::translate(glm::identity<glm::mat4>(),
                                       get_transform().get_position()) *
                        view);

    return view;
}

glm::mat4 camera::projection_matrix() const
{
    // TODO: optimize with caching
    if (_ortho_flag)
    {
        glm::quat rotation = get_transform().get_rotation();
        glm::vec3 direction = rotation * glm::vec3 { 0, 0, 1 };
        float dist =
            std::abs(glm::dot(direction, get_transform().get_position()));

        return glm::ortho(-_render_size.x / dist,
                          _render_size.x / dist,
                          -_render_size.y / dist,
                          _render_size.y / dist,
                          0.01f,
                          10000.0f);
    }

    return glm::perspective(
        glm::radians(_fov), _render_size.x / _render_size.y, 0.01f, 10000.0f);
}

transform& camera::get_transform() { return _transformation; }

const transform& camera::get_transform() const { return _transformation; }

camera* camera::active_camera() { return camera::_active_camera; }

const std::vector<camera*>& camera::all_cameras() { return camera::_cameras; }

camera* camera::_active_camera = nullptr;

std::vector<camera*> camera::_cameras;
