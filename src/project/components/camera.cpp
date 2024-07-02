#include <glm/gtx/matrix_decompose.hpp>
#include <nlohmann/json.hpp>

#include "project/components/camera.hpp"

#include "graphics/framebuffer.hpp"
#include "graphics/graphics_buffer.hpp"
#include "graphics/material.hpp"
#include "project/components/light.hpp"
#include "project/components/mesh_filter.hpp"
#include "project/components/mesh_renderer.hpp"
#include "project/components/transform.hpp"
#include "project/memory_manager.hpp"
#include "project/scene.hpp"
#include "project/serialization_utilities.hpp"
#include "project/serializer.hpp"
#include "project/serializer_json.hpp"
#include "renderer/renderer_3d.hpp"

using namespace serialization::utilities;

namespace components
{
camera::camera(game_object& obj)
    : component("camera", obj)
{
}

camera& camera::operator=(camera&& obj) = default;

camera::~camera() { std::cout << "~camera" << std::endl; }

void camera::set_fov(double fov)
{
    _field_of_view = fov;
    _projection_matrix_dirty = true;
}

double camera::get_fov() const { return _field_of_view; }

void camera::set_orthogonal(bool ortho_flag)
{
    _is_orthogonal = ortho_flag;
    _projection_matrix_dirty = true;
}

bool camera::is_orthogonal() const { return _is_orthogonal; }

double camera::get_aspect_ratio() const
{
    auto drs = glm::dvec2(get_render_size());
    return drs.x / drs.y;
}

void camera::set_active() { _active_camera = this; }

camera* camera::get_active() { return _active_camera; }

std::vector<camera*> camera::all() { return _cameras; }

void camera::set_render_size(size_t width, size_t height)
{
    set_render_size({ width, height });
}

void camera::set_render_size(glm::uvec2 size)
{
    if (_render_size != size)
    {
        _render_size = glm::max(glm::uvec2 { 1, 1 }, size);
        _framebuffer->resize(_render_size);
        _projection_matrix_dirty = true;
    }
}

void camera::get_render_size(size_t& width, size_t& height) const
{
    width = _render_size.x;
    height = _render_size.y;
}

glm::uvec2 camera::get_render_size() const { return _render_size; }

void camera::set_render_texture(std::weak_ptr<texture> render_texture)
{
    _user_render_texture = render_texture;
}

std::shared_ptr<texture> camera::get_render_texture() const
{
    return _user_render_texture.lock();
}

void camera::set_background(glm::dvec4 color) { _background_color = color; }

void camera::set_background(std::weak_ptr<texture> img)
{
    _background_texture = img;
}

glm::dvec4 camera::get_background_color() const
{
    return glm::dvec4(_background_color);
}

std::shared_ptr<texture> camera::get_background_texture() const
{
    return _background_texture.lock();
}

void camera::render()
{
    setup_lights();
    render_on_private_texture();

    _framebuffer->bind();
    render_texture_background();

    // if (get_gizmos_enabled())
    // {
    //     render_gizmos();
    // }

    _framebuffer->unbind();
    if (auto urt = _user_render_texture.lock())
    {
        _framebuffer->copy_texture(urt.get());
    }
}

glm::mat4 camera::projection_matrix() const { return _projection_matrix; }

glm::mat4 camera::view_matrix() const { return _view_matrix; }

glm::mat4 camera::vp_matrix() const
{
    return projection_matrix() * view_matrix();
}

template <>
void camera::serialize<json_serializer>(json_serializer& s)
{
    s.add_component(nlohmann::json {
        { "type", type_id<camera>() },
        { "is_enabled", is_enabled() },
    });
}

void camera::deserialize(const nlohmann::json& j)
{
    set_enabled(j[ "is_enabled" ]);
}

void camera::on_init()
{
    _view_matrix = glm::inverse(get_transform().get_matrix());
    _projection_matrix = calculate_projection_matrix();

    _framebuffer = std::make_unique<framebuffer>();
    _framebuffer->set_samples(32);
    _framebuffer->resize(_render_size);
    _framebuffer->initialize();
    _lights_buffer = std::make_unique<graphics_buffer>(
        graphics_buffer::type::shader_storage);
}

void camera::on_update()
{
    _view_matrix = glm::inverse(get_transform().get_matrix());

    if (_projection_matrix_dirty)
    {
        _projection_matrix = calculate_projection_matrix();
        _projection_matrix_dirty = false;
    }
}

void camera::set_projection_matrix(glm::mat4 mat)
{
    _projection_matrix = std::move(mat);
}

void camera::set_view_matrix(glm::mat4 mat) { _view_matrix = std::move(mat); }

void camera::render_texture_background()
{
    return;
    // TODO: As we no longer keep the transform, it will be way easier and
    // probably also cheaper to render a sphere instead of a quad

    // auto background_shader =
    //     asset_manager::default_asset_manager()->get_shader("camera_background");
    // _background_texture->set_active_texture(0);
    // background_shader->set_uniform("u_environment_map", 0);
    // background_shader->set_uniform("u_camera_matrix",
    //                                glm::toMat4(get_transform().get_rotation())
    //                                *
    //                                    glm::inverse(projection_matrix()));
    // if (_background_texture)
    // {
    //     _background_texture->set_active_texture(0);
    // }

    // background_shader->use();
    // mesh* quad_mesh =
    // asset_manager::default_asset_manager()->get_mesh("quad");
    // quad_mesh->render();
    // shader_program::unuse();
}

void camera::render_on_private_texture() const
{
    _framebuffer->bind();
    // TODO?: maybe better to clear with the specified background color
    // instead
    // of drawing background quad with that color
    glClearColor(
        _background_color.x, _background_color.y, _background_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (scene::get_active_scene())
    {
        scene::get_active_scene()->visit_root_objects(
            [ this ](auto& obj)
        {
            if (!obj->is_active())
                return;

            if (auto* renderer =
                    obj->template try_get<components::mesh_renderer>())
            {
                auto* mesh =
                    obj->template get<components::mesh_filter>().get_mesh();
                auto* material = renderer->get_material();
                if (material)
                {
                    material->set_property_value(
                        "u_model_matrix",
                        glm::mat4(obj->get_transform().get_matrix()));
                    material->set_property_value("u_vp_matrix",
                                                 glm::mat4(vp_matrix()));
                    material->set_property_value(
                        "u_camera_position",
                        glm::vec3(get_transform().get_position()));
                    renderer_3d().draw_mesh(mesh, material);
                }
            }
        });
    }
    _framebuffer->unbind();
}

void camera::setup_lights()
{
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
    size_t i = 0;

    scene::get_active_scene()->visit_root_objects(
        [ &i, &glsl_lights ](auto& obj)
    {
        if (auto* light = obj->try_get<components::light>())
        {
            glsl_lights.push_back({});
            auto& glight = glsl_lights.back();
            glight.position = light->get_transform().get_position();
            glight.direction = glm::normalize(
                light->get_transform().get_rotation() * glm::dvec3 { 0, 0, 1 });
            glight.color = light->get_color();
            glight.intensity = light->get_intensity();
            glight.radius = light->get_radius();
            glight.type = static_cast<uint32_t>(light->get_type());
            ++i;
        }
    });

    _lights_buffer->set_element_stride(4 * 3 * sizeof(float));
    _lights_buffer->set_element_count(glsl_lights.size());
    _lights_buffer->set_usage_type(graphics_buffer::usage_type::dynamic_copy);
    _lights_buffer->set_data(glsl_lights.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _lights_buffer->get_handle());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _lights_buffer->get_handle());
}

glm::mat4 camera::calculate_projection_matrix() const
{
    // TODO: optimize with caching
    // copy into floating point vec2
    glm::dvec2 size = _render_size;
    if (_is_orthogonal)
    {
        glm::dquat rotation = get_transform().get_rotation();
        glm::dvec3 direction = rotation * glm::dvec3 { 0, 0, 1 };
        float dist =
            std::abs(glm::dot(direction, get_transform().get_position()));

        return glm::ortho(-size.x / dist,
                          size.x / dist,
                          -size.y / dist,
                          size.y / dist,
                          0.01,
                          10000.0);
    }

    return glm::perspective(_field_of_view, size.x / size.y, 0.1, 10000.0);
}

camera* camera::_active_camera { nullptr };

std::vector<camera*> camera::_cameras;
} // namespace components
