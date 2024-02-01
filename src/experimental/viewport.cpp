#include <glad/gl.h>
#include <glm/vec2.hpp>

#include "experimental/viewport.hpp"

#include "asset_manager.hpp"
#include "camera.hpp"
#include "image.hpp"
#include "texture.hpp"

namespace experimental
{

struct viewport::viewport_private
{
    glm::vec2 _position;
    glm::vec2 _size;
    texture _surface_texture;
    std::weak_ptr<camera> _camera;
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
    _p->_surface_texture.reinit(usize.x, usize.y);
}

glm::vec2 viewport::get_size() const { return _p->_size; }

void viewport::set_camera(std::weak_ptr<camera> cam) { _p->_camera = cam; }

std::shared_ptr<camera> viewport::get_camera() const
{
    return _p->_camera.lock();
}

void viewport::render()
{
    auto cam = get_camera();
    if (cam == nullptr)
    {
        return;
    }

    glViewport(get_position().x, get_position().y, get_size().x, get_size().y);
    cam->set_render_size(get_size());
    cam->set_render_texture(&_p->_surface_texture);
    cam->render();
}

void viewport::take_screenshot(std::string_view path)
{
    auto screenshot = image::from_texture(&_p->_surface_texture);
    asset_manager::default_asset_manager()->save_asset(path, screenshot);
    delete screenshot;
}

} // namespace experimental
