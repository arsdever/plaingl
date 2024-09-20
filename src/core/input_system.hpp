#pragma once

#include <glm/fwd.hpp>

#include "common/event.hpp"
#include "core/inputs/gamepad.hpp"

namespace core
{
class binding;

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

    enum modifiers
    {
        None = 0,
        Shift = 1,
        Control = 2,
        Alt = 4,
        Super = 8,
        CapsLock = 16,
        NumLock = 32,
    };

    enum joystick_input
    {
        l1,
        l2,
        r1,
        r2,
        arrow_up,
        arrow_down,
        arrow_left,
        arrow_right,
        left_analog_up,
        left_analog_down,
        left_analog_left,
        left_analog_right,
        right_analog_up,
        right_analog_down,
        right_analog_left,
        right_analog_right,
        share,
        options,
        home,
        trackpad,
        x,
        circle,
        square,
        triangle
    };

    static void update_device_list();

    static void set_mouse_position(glm::ivec2 pos);
    static glm::ivec2 get_mouse_position();
    static glm::ivec2 get_mouse_delta();
    static button_state get_mouse_button(mouse_button button_code);
    static void set_mouse_button(mouse_button button_code, button_state state);

    static bool is_key_down(int keycode);
    static void set_key_down(int keycode, bool state = true);

    static void set_modifiers(modifiers modifiers);
    static modifiers get_modifiers();

    static std::shared_ptr<binding> setup_binding(std::string path,
                                                  std::string name);

    static event<void(int)> on_keypress;

private:
    static std::unordered_set<int> _pressed_keys;
    static std::unordered_map<mouse_button, button_state> _mouse_buttons_state;
    static glm::ivec2 _mouse_position;
    static glm::ivec2 _mouse_delta;
    static modifiers _modifiers;
    static std::vector<inputs::gamepad> _gamepads;
    static std::unordered_map<std::string, std::weak_ptr<binding>> _mapping;
};
} // namespace core
