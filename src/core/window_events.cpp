#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "core/window_events.hpp"

#include "logging.hpp"

// TODO: some logic may not work in case of simultaneous multiple button actions

namespace
{
logger log() { return get_logger("events"); }
} // namespace

namespace core
{

window_event::window_event(type t, window* sender)
    : _type(t)
    , _sender(sender)
{
}

window_event::~window_event() = default;

window_event::type window_event::get_type() const { return _type; }

window* window_event::get_sender() const { return _sender; }

render_event::render_event(type t, window* sender)
    : window_event(t, sender)
{
}

input_event::input_event(type t, modifiers mod, window* sender)
    : window_event(t, sender)
    , _modifiers(mod)
{
}

input_event::modifiers input_event::get_modifiers() const { return _modifiers; }

single_point_event::single_point_event(type t,
                                       const glm::vec2& local_pos,
                                       const glm::vec2& scene_pos,
                                       const glm::vec2& global_pos,
                                       int button,
                                       int buttons,
                                       modifiers mods,
                                       window* sender)
    : input_event(t, mods, sender)
    , _local_pos(local_pos)
    , _scene_pos(scene_pos)
    , _global_pos(global_pos)
    , _button(button)
    , _buttons(buttons)
{
}

int single_point_event::get_button() const { return _button; }

int single_point_event::get_buttons() const { return _buttons; }

glm::vec2 single_point_event::get_local_position() const { return _local_pos; }

glm::vec2 single_point_event::get_scene_position() const { return _scene_pos; }

glm::vec2 single_point_event::get_global_position() const
{
    return _global_pos;
}

mouse_event::mouse_event(type t,
                         const glm::vec2& local_pos,
                         const glm::vec2& scene_pos,
                         const glm::vec2& global_pos,
                         int button,
                         int buttons,
                         modifiers mods,
                         window* sender)
    : single_point_event(
          t, local_pos, scene_pos, global_pos, button, buttons, mods, sender)
{
}

wheel_event::wheel_event(type t,
                         const glm::vec2& local_pos,
                         const glm::vec2& scene_pos,
                         const glm::vec2& global_pos,
                         int button,
                         int buttons,
                         modifiers mods,
                         glm::vec2 delta,
                         bool inverted,
                         window* sender)
    : single_point_event(
          t, local_pos, scene_pos, global_pos, button, buttons, mods, sender)
    , _delta(std::move(delta))
    , _is_inverted(inverted)
{
}

glm::vec2 wheel_event::get_delta() const { return _delta; }

bool wheel_event::get_is_inverted() const { return _is_inverted; };

enter_event::enter_event(type t,
                         const glm::vec2& local_pos,
                         const glm::vec2& scene_pos,
                         const glm::vec2& global_pos,
                         int button,
                         int buttons,
                         modifiers mods,
                         window* sender)
    : single_point_event(
          t, local_pos, scene_pos, global_pos, button, buttons, mods, sender)
{
}

leave_event::leave_event(window* sender)
    : window_event(type::Leave, sender)
{
}

close_event::close_event(window* sender)
    : window_event(type::Close, sender)
{
}

resize_event::resize_event(glm::vec2 old_size, glm::vec2 size, window* sender)
    : window_event(type::Resize, sender)
    , _old_size(old_size)
    , _new_size(size)
{
}

glm::vec2 resize_event::get_old_size() const { return _old_size; }

glm::vec2 resize_event::get_new_size() const { return _new_size; }

move_event::move_event(glm::vec2 old_position,
                       glm::vec2 position,
                       window* sender)
    : window_event(type::Move, sender)
    , _old_position(old_position)
    , _new_position(position)
{
}

glm::vec2 move_event::get_old_position() const { return _old_position; }

glm::vec2 move_event::get_new_position() const { return _new_position; }

key_event::key_event(type t,
                     int scancode,
                     int key,
                     modifiers mods,
                     bool repeated,
                     window* sender)
    : input_event(t, mods, sender)
    , _scancode(scancode)
    , _key(key)
    , _is_repeated(repeated)
{
}

int key_event::get_scancode() const { return _scancode; }

int key_event::get_key() const { return _key; }

bool key_event::get_is_repeated() const { return _is_repeated; }

} // namespace core
