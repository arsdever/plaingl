#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "experimental/window_events.hpp"

#include "logging.hpp"

// TODO: some logic may not work in case of simultaneous multiple button actions
// TODO: implement double click actions

namespace
{
logger log() { return get_logger("events"); }
} // namespace

namespace experimental
{

window_event::window_event(type t)
    : _type(t)
{
}

window_event::~window_event() = default;

window_event::type window_event::get_type() const { return _type; }

input_event::input_event(type t, modifiers mod)
    : window_event(t)
    , _modifiers(mod)
{
}

mouse_event::mouse_event(type t,
                         const glm::vec2& local_pos,
                         const glm::vec2& scene_pos,
                         const glm::vec2& global_pos,
                         int button,
                         int buttons,
                         modifiers mods)
    : input_event(t, mods)
    , _local_pos(local_pos)
    , _scene_pos(scene_pos)
    , _global_pos(global_pos)
    , _button(button)
    , _buttons(buttons)
{
}

int mouse_event::get_button() const { return _button; }

int mouse_event::get_buttons() const { return _buttons; }

glm::vec2 mouse_event::get_local_position() const { return _local_pos; }

glm::vec2 mouse_event::get_scene_position() const { return _scene_pos; }

glm::vec2 mouse_event::get_global_position() const { return _global_pos; }

} // namespace experimental
