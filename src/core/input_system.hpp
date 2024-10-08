#pragma once

#include <glm/fwd.hpp>

#include "core/core_fwd.hpp"

#include "common/event.hpp"
#include "common/utils.hpp"
#include "core/inputs/gamepad.hpp"

namespace core
{
class input_system
{
public:
    enum mouse_button
    {
        button_0,
        button_1,
        button_2,
        button_3,
        button_4,
        button_5,
        button_6,
        button_7,
        button_8,
        button_9,
        left = button_0,
        right = button_1,
        middle = button_2,
    };

    enum button_state
    {
        unspecified,
        press,
        hold,
        release,
    };

    enum modifiers
    {
        none = 0,
        shift = 1,
        control = 2,
        alt = 4,
        super = 8,
        caps_lock = 16,
        num_lock = 32,
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

    static void set_input_source(std::shared_ptr<window> input_source);
    static std::shared_ptr<window> get_input_source();

    static void set_mouse_position(glm::ivec2 pos);
    static glm::ivec2 get_mouse_position();
    static glm::ivec2 get_mouse_delta();
    static button_state get_mouse_button(mouse_button button_code);
    static void set_mouse_button(mouse_button button_code, button_state state);

    static bool is_key_down(int keycode);
    static void set_key_down(int keycode, bool state = true);

    static void set_modifiers(modifiers modifiers);
    static modifiers get_modifiers();

    static std::shared_ptr<binding> declare_input(std::string_view name);
    static std::shared_ptr<binding> get_input(std::string_view name);
    static std::shared_ptr<binding> bind_input(std::shared_ptr<binding> bond,
                                               std::string_view path);
    static std::shared_ptr<binding> bind_input(std::string_view name,
                                               std::string_view path);

    static event<void(int)> on_keypress;

private:
    static std::unordered_set<int> _pressed_keys;
    static std::unordered_map<mouse_button, button_state> _mouse_buttons_state;
    static glm::ivec2 _mouse_position;
    static glm::ivec2 _mouse_delta;
    static modifiers _modifiers;
    static std::vector<inputs::gamepad> _gamepads;
    static std::unordered_map<std::string,
                              std::shared_ptr<binding>,
                              string_hash,
                              std::equal_to<>>
        _mapping;
    static std::weak_ptr<window> _input_source;
};
} // namespace core
