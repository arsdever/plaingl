#include "viewport.hpp"

#include "camera.hpp"
#include "game_object.hpp"
#include "gizmo_drawer.hpp"
#include "logging.hpp"
#include "scene.hpp"

namespace
{
static inline logger log() { return get_logger("viewport"); }
} // namespace

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

camera* viewport::get_camera() const { return _viewport_camera; }

void viewport::set_camera(camera* viewport_camera)
{
    _viewport_camera = viewport_camera;
}

void viewport::set_name(std::string_view name) { _name = name; }

std::string_view viewport::get_name() { return _name; }

void viewport::update()
{
    glViewport(static_cast<int>(_position.x),
               static_cast<int>(_position.y),
               static_cast<int>(_resolution.x),
               static_cast<int>(_resolution.y));

    _viewport_camera->set_active();
    _viewport_camera->set_render_size(static_cast<float>(_resolution.x),
                                      static_cast<float>(_resolution.y));
    if (_resolution.x == 0 || _resolution.y == 0)
    {
        log()->warn("Viewport ({}) size is too small. Skipping rendering.",
                    _name);
        return;
    }
    draw();
}

void viewport::draw() const
{
    _viewport_camera->render();

    if (const auto* s = scene::get_active_scene())
    {
        for (auto* obj : s->objects())
        {
            if (!obj->is_active())
            {
                continue;
            }
            gizmo_drawer::instance()->get_shader().set_uniform(
                "model_matrix",
                std::make_tuple(obj->get_transform().get_matrix()));
            obj->draw_gizmos();
        }
    }
}
