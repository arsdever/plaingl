#include <GLFW/glfw3.h>

#include "input_system.hpp"

#include "logging.hpp"

namespace
{
inline logger log() { get_logger("input_system"); }
} // namespace

bool input_system::is_key_down(int keycode)
{
    auto* wnd = glfwGetCurrentContext();
    if (!wnd)
    {
        return false;
    }

    return glfwGetKey(wnd, keycode) == GLFW_PRESS;
}
