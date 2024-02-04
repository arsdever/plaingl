#include <GLFW/glfw3.h>

#include "input_system.hpp"

#include "logging.hpp"

namespace
{
inline logger log() { return get_logger("input_system"); }
} // namespace

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

event<void(int)> input_system::on_keypress;
std::unordered_set<int> input_system::_pressed_keys;
