#include "camera.hpp"

#include "asset_manager.hpp"
#include "components/renderer_component.hpp"
#include "framebuffer.hpp"
#include "game_object.hpp"
#include "gizmo_drawer.hpp"
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

    _background_shader = std::make_unique<shader_program>();
    _background_shader->init();
    _background_shader->add_shader("resources/camera_background_shader.vert");
    _background_shader->add_shader("resources/camera_background_shader.frag");
    _background_shader->link();

    _framebuffer = std::make_unique<framebuffer>();
    _framebuffer->set_samples(32);
    _framebuffer->resize(_render_size);
    _framebuffer->initialize();
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

void camera::set_gizmos_enabled(bool flag) { _gizmos_enabled = flag; }

bool camera::get_gizmos_enabled() const { return _gizmos_enabled; }

void camera::set_background(glm::vec3 color)
{
    _background_color = color;
    _background_shader->set_uniform("background_color", color);
    _background_shader->set_uniform("use_color", 1.0f);
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
        _background_shader->set_uniform("cubemap", _background_texture.get());
        _background_shader->set_uniform("use_color", 0.0f);
    }
}

void camera::render()
{
    auto* old_active_camera = set_active();

    setup_lights();
    render_on_private_texture();

    _background_shader->set_uniform(
        "camera_matrix",
        glm::toMat4(get_transform().get_rotation()) *
            glm::inverse(projection_matrix()));
    if (_background_texture)
    {
        _background_texture->set_active_texture(0);
    }

    _background_shader->use();
    mesh* quad_mesh = asset_manager::default_asset_manager()->get_mesh("quad");
    quad_mesh->render();
    shader_program::unuse();

    if (get_gizmos_enabled())
    {
        render_gizmos();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (auto urt = _user_render_texture.lock())
    {
        _framebuffer->copy_texture(urt.get());
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
    glm::vec3 cam_right = rotation * glm::vec3 { 1, 0, 0 };
    glm::vec3 cam_up = rotation * glm::vec3 { 0, 1, 0 };
    glm::vec3 cam_forward = rotation * glm::vec3 { 0, 0, 1 };

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

    return glm::perspective(_fov, size.x / size.y, 0.1f, 10000.0f);
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
                    "u_model_matrix", obj->get_transform().get_matrix());
                renderer->render();
            }
        }
    }
    _framebuffer->unbind();
}

void camera::render_gizmos() const
{
    _framebuffer->bind();
    glEnable(GL_BLEND);
    if (scene::get_active_scene())
    {
        for (auto* obj : scene::get_active_scene()->objects())
        {
            if (!obj->is_active())
            {
                continue;
            }
            gizmo_drawer::instance()->get_shader().set_uniform(
                "u_model_matrix", obj->get_transform().get_matrix());
            obj->draw_gizmos();
        }
    }
    glDisable(GL_BLEND);
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

    _lights_buffer.set_element_stride(4 * 3 * sizeof(float));
    _lights_buffer.set_element_count(glsl_lights.size());
    _lights_buffer.set_usage_type(graphics_buffer::usage_type::dynamic_copy);
    _lights_buffer.set_data(glsl_lights.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _lights_buffer.get_handle());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _lights_buffer.get_handle());
}

camera* camera::_active_camera = nullptr;

std::vector<camera*> camera::_cameras;
