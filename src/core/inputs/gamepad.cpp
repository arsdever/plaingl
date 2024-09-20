#include <GLFW/glfw3.h>

#include "core/inputs/gamepad.hpp"

namespace core::inputs
{
gamepad::gamepad(int id)
    : _joystick_index(id)
{
}

bool gamepad::get_button(buttons btn_code)
{
    update_state();
    return _state[ btn_code ];
}

float gamepad::get_left_trigger()
{
    update_state();
    auto state = reinterpret_cast<GLFWgamepadstate*>(_state.data());
    return state->axes[ 4 ];
}

float gamepad::get_right_trigger()
{
    update_state();
    auto state = reinterpret_cast<GLFWgamepadstate*>(_state.data());
    return state->axes[ 5 ];
}

glm::vec2 gamepad::get_left_axis()
{
    update_state();
    auto state = reinterpret_cast<GLFWgamepadstate*>(_state.data());
    return { state->axes[ 0 ], state->axes[ 1 ] };
}

glm::vec2 gamepad::get_right_axis()
{
    update_state();
    auto state = reinterpret_cast<GLFWgamepadstate*>(_state.data());
    return { state->axes[ 2 ], state->axes[ 3 ] };
}

void gamepad::update_state()
{
    glfwGetGamepadState(_joystick_index,
                        reinterpret_cast<GLFWgamepadstate*>(_state.data()));
}
} // namespace core::inputs
