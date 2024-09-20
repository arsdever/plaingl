#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "core/input_system.hpp"

#include "common/logging.hpp"
#include "common/utils.hpp"
#include "core/inputs/binding.hpp"
#include "core/window.hpp"

namespace core
{
namespace
{
inline logger log() { return get_logger("input_system"); }
} // namespace

void input_system::update_device_list()
{
    for (int i = 0; i < GLFW_JOYSTICK_LAST; i++)
    {
        if (glfwJoystickIsGamepad(i))
        {
            std::string uid = glfwGetJoystickGUID(i);
            std::string name = glfwGetJoystickName(i);
            log()->info("Joystick {} present: {}: {}", i, uid, name);
            _gamepads.push_back({ i });
        }
    }
}

void input_system::set_mouse_position(glm::ivec2 pos)
{
    _mouse_delta = pos - _mouse_position;
    _mouse_position = pos;
}

glm::ivec2 input_system::get_mouse_position() { return _mouse_position; }

glm::ivec2 input_system::get_mouse_delta() { return _mouse_delta; }

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

void input_system::set_modifiers(modifiers modifiers)
{
    _modifiers = modifiers;
}

input_system::modifiers input_system::get_modifiers() { return _modifiers; }

std::shared_ptr<binding> input_system::setup_binding(std::string path,
                                                     std::string name)
{
    size_t pos = path.find_first_of(".");
    std::string device = path.substr(0, pos);
    std::shared_ptr<binding> result;
    if (device == "mouse") { }
    else if (device == "keyboard") { }
    else if (device == "gamepad")
    {
        std::string device_input = path.substr(pos + 1);
        if (device_input == "left_joystick")
        {
            result = std::make_shared<binding>([]() -> glm::vec2
            { return _gamepads[ 0 ].get_left_axis(); });
            auto [ _1, _2 ] = _mapping.emplace(name, result);
        }
    }

    return result;
}

// void input_system::get_value(std::string property) { }

std::unordered_set<int> input_system::_pressed_keys;
std::unordered_map<input_system::mouse_button, input_system::button_state>
    input_system::_mouse_buttons_state;
event<void(int)> input_system::on_keypress;
glm::ivec2 input_system::_mouse_position;
glm::ivec2 input_system::_mouse_delta;
input_system::modifiers input_system::_modifiers;
std::vector<inputs::gamepad> input_system::_gamepads;
std::unordered_map<std::string, std::weak_ptr<binding>> input_system::_mapping;
} // namespace core
