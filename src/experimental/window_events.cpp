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

input_event::modifiers input_event::get_modifiers() const { return _modifiers; }

single_point_event::single_point_event(type t,
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
                         modifiers mods)
    : single_point_event(
          t, local_pos, scene_pos, global_pos, button, buttons, mods)
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
                         bool inverted)
    : single_point_event(
          t, local_pos, scene_pos, global_pos, button, buttons, mods)
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
                         modifiers mods)
    : single_point_event(
          t, local_pos, scene_pos, global_pos, button, buttons, mods)
{
}

leave_event::leave_event()
    : window_event(type::Leave)
{
}

close_event::close_event()
    : window_event(type::Close)
{
}

resize_event::resize_event(glm::vec2 old_size, glm::vec2 size)
    : window_event(type::Resize)
    , _old_size(old_size)
    , _new_size(size)
{
}

glm::vec2 resize_event::get_old_size() const { return _old_size; }

glm::vec2 resize_event::get_new_size() const { return _new_size; }

move_event::move_event(glm::vec2 old_position, glm::vec2 position)
    : window_event(type::Move)
    , _old_position(old_position)
    , _new_position(position)
{
}

glm::vec2 move_event::get_old_position() const { return _old_position; }

glm::vec2 move_event::get_new_position() const { return _new_position; }

key_event::key_event(type t, int scancode, modifiers mods, bool repeated)
    : input_event(t, mods)
    , _scancode(scancode)
    , _is_repeated(repeated)
{
}

int key_event::get_scancode() const { return _scancode; }

bool key_event::get_is_repeated() const { return _is_repeated; }

} // namespace experimental
