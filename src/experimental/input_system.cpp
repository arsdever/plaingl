#include <glm/vec2.hpp>

#include "experimental/input_system.hpp"

#include "logging.hpp"

namespace
{
inline logger log() { return get_logger("input_system"); }
} // namespace

namespace experimental
{

void input_system::set_mouse_position(glm::ivec2 pos) { _mouse_position = pos; }

glm::ivec2 input_system::get_mouse_position() { return _mouse_position; }

input_system::button_state
input_system::get_mouse_button(mouse_button button_code)
{
    auto it = _mouse_buttons_state.find(button_code);
    if (it == _mouse_buttons_state.end())
        return button_state::Unspecified;

    return it->second;
}

void input_system::set_mouse_button(mouse_button button_code,
                                    button_state state)
{
    _mouse_buttons_state[ button_code ] = state;
}

bool input_system::is_key_down(int keycode)
{
    return _pressed_keys.contains(keycode);
}

void input_system::set_key_down(int keycode, bool state)
{
    if (state)
    {
        _pressed_keys.emplace(keycode);
        on_keypress(keycode);
    }
    else
    {
        _pressed_keys.erase(keycode);
    }
}

std::unordered_set<int> input_system::_pressed_keys;
std::unordered_map<input_system::mouse_button, input_system::button_state>
    input_system::_mouse_buttons_state;
event<void(int)> input_system::on_keypress;
glm::ivec2 input_system::_mouse_position;

} // namespace experimental
