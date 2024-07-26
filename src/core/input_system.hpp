#pragma once

#include <unordered_map>
#include <unordered_set>

#include <glm/fwd.hpp>

#include "event.hpp"

namespace core
{

class input_system
{
public:
    enum mouse_button
    {
        MouseButton0,
        MouseButton1,
        MouseButton2,
        MouseButton3,
        MouseButton4,
        MouseButton5,
        MouseButton6,
        MouseButton7,
        MouseButton8,
        MouseButton9,
        MouseButtonLeft = MouseButton0,
        MouseButtonRight = MouseButton1,
        MouseButtonMiddle = MouseButton2,
    };

    enum button_state
    {
        Unspecified,
        Press,
        Hold,
        Release,
    };

    static void set_mouse_position(glm::ivec2 pos);
    static glm::ivec2 get_mouse_position();
    static button_state get_mouse_button(mouse_button button_code);
    static void set_mouse_button(mouse_button button_code, button_state state);

    static bool is_key_down(int keycode);
    static void set_key_down(int keycode, bool state = true);

    static event<void(int)> on_keypress;

private:
    static std::unordered_set<int> _pressed_keys;
    static std::unordered_map<mouse_button, button_state> _mouse_buttons_state;
    static glm::ivec2 _mouse_position;
};

} // namespace core
