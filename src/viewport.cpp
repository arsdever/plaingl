#include "viewport.hpp"

#include "camera.hpp"
#include "game_object.hpp"
#include "gizmo_drawer.hpp"
#include "logging.hpp"
#include "scene.hpp"
#include "window.hpp"

namespace
{
inline logger log() { return get_logger("viewport"); }
} // namespace

viewport::viewport()
    : _viewport_camera { std::make_unique<camera>() }
{
    _viewport_camera->get_transform().set_position({ 5, 5, 5 });
    _viewport_camera->get_transform().set_rotation(glm::quatLookAt(
        glm::normalize(_viewport_camera->get_transform().get_position()),
        { 0, 0, 1 }));
}

void viewport::init()
{
    // So far nothing to initialzie
}

size_t viewport::get_width() const { return _resolution.x; }

size_t viewport::get_height() const { return _resolution.y; }

glm::uvec2 viewport::get_resolution() const { return _resolution; }

void viewport::set_size(size_t width, size_t height)
{
    set_size(glm::uvec2 { width, height });
}

void viewport::set_size(glm::uvec2 size) { _resolution = size; }

glm::uvec2 viewport::get_position() const { return _position; }

void viewport::set_position(size_t x, size_t y)
{
    set_position(glm::uvec2 { x, y });
}

void viewport::set_position(glm::uvec2 position) { _position = position; }

camera* viewport::get_camera() const { return _user_camera; }

void viewport::set_camera(camera* viewport_camera)
{
    _user_camera = viewport_camera;
}

void viewport::set_name(std::string_view name) { _name = name; }

std::string_view viewport::get_name() const { return _name; }

void viewport::set_visible(bool visible_flag) { _visible_flag = visible_flag; }

bool viewport::is_visible() const { return _visible_flag; }

void viewport::set_window(window* owning_window) { _window = owning_window; }

window* viewport::get_window() const { return _window; }

void viewport::update()
{
    if (!is_visible())
    {
        return;
    }

    if (_resolution.x == 0 || _resolution.y == 0)
    {
        log()->warn("Viewport ({}) size is too small. Skipping rendering.",
                    _name);
        return;
    }

    glViewport(
        static_cast<int>(_position.x),
        static_cast<int>(_window->get_height() - _position.y - _resolution.y),
        static_cast<int>(_resolution.x),
        static_cast<int>(_resolution.y));

    render_camera()->set_active();
    render_camera()->set_render_size(static_cast<float>(_resolution.x),
                                     static_cast<float>(_resolution.y));
    draw();
}

camera* viewport::render_camera() const
{
    return _user_camera ? _user_camera : _viewport_camera.get();
}

void viewport::draw() const
{
    render_camera()->render();

    glEnable(GL_BLEND);
    if (const auto* s = scene::get_active_scene())
    {
        for (auto* obj : s->objects())
        {
            if (!obj->is_active())
            {
                continue;
            }
            gizmo_drawer::instance()->get_shader().set_uniform(
                "model_matrix", obj->get_transform().get_matrix());
            obj->draw_gizmos();
        }
    }
    glDisable(GL_BLEND);
}
