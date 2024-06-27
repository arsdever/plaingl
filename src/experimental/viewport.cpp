#include <glad/gl.h>
#include <glm/vec2.hpp>

#include "experimental/viewport.hpp"

#include "asset_manager.hpp"
#include "experimental/window.hpp"
#include "image.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "project/components/camera.hpp"
#include "renderer/renderer_3d.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace
{
void render_quad(texture* _p)
{
    material* surface_mat =
        asset_manager::default_asset_manager()->get_material("surface");
    mesh* quad_mesh = asset_manager::default_asset_manager()->get_mesh("quad");
    surface_mat->set_property_value("u_image", _p);
    renderer_3d().draw_mesh(quad_mesh, surface_mat);
    shader_program::unuse();
}
} // namespace

namespace experimental
{

struct viewport::viewport_private
{
    glm::vec2 _position { 0, 0 };
    glm::vec2 _size { 0, 0 };
    std::shared_ptr<texture> _surface_texture { std::make_shared<texture>() };
};

viewport::viewport() { _p = std::make_unique<viewport_private>(); }

viewport::~viewport() = default;

void viewport::initialize() { }

void viewport::set_position(glm::vec2 pos) { _p->_position = pos; }

glm::vec2 viewport::get_position() const { return _p->_position; }

void viewport::set_size(glm::vec2 size)
{
    _p->_size = size;
    glm::uvec2 usize = size;
}

glm::vec2 viewport::get_size() const { return _p->_size; }

void viewport::render()
{
    _current_viewport = this;
    auto cam = components::camera::get_active();
    if (cam == nullptr)
    {
        return;
    }

    glViewport(0, 0, get_size().x, get_size().y);
    cam->set_render_size(get_size());
    cam->set_render_texture(_p->_surface_texture);
    cam->render();

    glViewport(get_position().x, get_position().y, get_size().x, get_size().y);
    // TODO: move to renderer
    render_quad(_p->_surface_texture.get());
}

void viewport::take_screenshot(std::string_view path)
{
    auto screenshot = image::from_texture(_p->_surface_texture.get());
    asset_manager::default_asset_manager()->save_asset(path, screenshot);
    delete screenshot;
}

viewport* viewport::current_viewport() { return _current_viewport; }

viewport* viewport::_current_viewport { nullptr };

} // namespace experimental
