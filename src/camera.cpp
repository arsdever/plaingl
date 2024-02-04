/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "camera.hpp"

#include "components/renderer_component.hpp"
#include "framebuffer.hpp"
#include "game_object.hpp"
#include "light.hpp"
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

    _framebuffer = std::make_unique<framebuffer>();
    _framebuffer->resize(_render_size);
    _framebuffer->initialize();
    glGenBuffers(1, &_lights_buffer);
    set_background(glm::vec3 { 0.0f, 0.0f, 0.0f });
}

camera::~camera() { std::erase(_cameras, this); }

void camera::set_fov(float fov) { _fov = fov; }

float camera::get_fov() const { return _fov; }

void camera::set_ortho(bool ortho_flag) { _ortho_flag = ortho_flag; }

float camera::get_aspect_ratio() const
{
    glm::vec2 size = _render_size;
    return size.x / size.y;
}

camera* camera::set_active()
{
    auto* old = _active_camera;
    _active_camera = this;
    return old;
}

void camera::set_render_size(size_t width, size_t height)
{
    set_render_size({ width, height });
}

void camera::set_render_size(glm::uvec2 size)
{
    glm::uvec2 new_size = glm::max(glm::uvec2 { 1, 1 }, size);
    if (_render_size == new_size)
    {
        return;
    }

    _render_size = new_size;
    _framebuffer->resize(_render_size);
}

void camera::set_render_texture(std::weak_ptr<texture> render_texture)
{
    _user_render_texture = render_texture;
}

std::shared_ptr<texture> camera::get_render_texture() const
{
    return _user_render_texture.lock();
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

    render_on_private_texture();
    setup_lights();

    _background_shader->set_uniform(
        "camera_matrix",
        std::make_tuple(glm::toMat4(get_transform().get_rotation()) *
                        glm::inverse(projection_matrix())));
    if (_background_texture)
    {
        _background_texture->set_active_texture(0);
    }

    _background_shader->use();
    _background_quad->render();
    shader_program::unuse();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (auto urt = _user_render_texture.lock())
    {
        urt->clone(_framebuffer->color_texture().get());
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
    // copy into floating point vec2
    glm::vec2 size = _render_size;
    if (_ortho_flag)
    {
        glm::quat rotation = get_transform().get_rotation();
        glm::vec3 direction = rotation * glm::vec3 { 0, 0, 1 };
        float dist =
            std::abs(glm::dot(direction, get_transform().get_position()));

        return glm::ortho(-size.x / dist,
                          size.x / dist,
                          -size.y / dist,
                          size.y / dist,
                          0.01f,
                          10000.0f);
    }

    return glm::perspective(
        glm::radians(_fov), size.x / size.y, 0.1f, 10000.0f);
}

transform& camera::get_transform() { return _transformation; }

const transform& camera::get_transform() const { return _transformation; }

camera* camera::active_camera() { return camera::_active_camera; }

const std::vector<camera*>& camera::all_cameras() { return camera::_cameras; }

void camera::render_on_private_texture() const
{
    _framebuffer->bind();
    // TODO?: maybe better to clear with the specified background color instead
    // of drawing background quad with that color
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    _framebuffer->unbind();
}

void camera::setup_lights()
{
    const auto& lights = light::get_all_lights();
    struct glsl_lights_t
    {
        glm::vec3 position;
        float intensity;
        glm::vec3 direction;
        float radius;
        glm::vec3 color;
        uint32_t type;
    };

    std::vector<glsl_lights_t> glsl_lights;
    glsl_lights.resize(lights.size());
    size_t i = 0;

    auto size_calculated = sizeof(glsl_lights_t);
    auto size_1 = (4 * 3 * sizeof(float)) * glsl_lights.size();

    for (const auto& light : lights)
    {
        glsl_lights[ i ].position = light->get_transform().get_position();
        glsl_lights[ i ].direction = glm::normalize(
            light->get_transform().get_rotation() * glm::vec3 { 0, 0, 1 });
        glsl_lights[ i ].color = light->get_color();
        glsl_lights[ i ].intensity = light->get_intensity();
        glsl_lights[ i ].radius = light->get_radius();
        glsl_lights[ i ].type = static_cast<uint32_t>(light->get_type());
        ++i;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _lights_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 (4 * 3 * sizeof(float)) * glsl_lights.size(),
                 glsl_lights.data(),
                 GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _lights_buffer);
}

camera* camera::_active_camera = nullptr;

std::vector<camera*> camera::_cameras;
