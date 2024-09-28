#include <glm/vec2.hpp>

#include "core/viewport.hpp"

#include "core/window.hpp"
#include "graphics/texture.hpp"
#include "project/components/camera.hpp"

namespace core
{

struct viewport::viewport_private
{
    glm::vec2 _position { 0, 0 };
    glm::vec2 _size { 0, 0 };
    std::shared_ptr<graphics::texture> _surface_texture {
        std::make_shared<graphics::texture>()
    };
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

void viewport::take_screenshot(std::string_view path)
{
    // assets::asset_manager::save_asset(path, screenshot);
}

viewport* viewport::current_viewport() { return _current_viewport; }

viewport* viewport::_current_viewport { nullptr };

} // namespace core
