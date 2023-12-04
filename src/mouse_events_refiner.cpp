#include <GLFW/glfw3.h>

#include "mouse_events_refiner.hpp"

mouse_events_refiner::mouse_events_refiner() = default;

void mouse_events_refiner::button_function(GLFWwindow* window,
                                           int button,
                                           int action,
                                           int mods)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    gl_window* wind = static_cast<gl_window*>(glfwGetWindowUserPointer(window));
    click(wind, glm::vec2{x, y});
}

void mouse_events_refiner::position_function(GLFWwindow* window,
                                             double x_position,
                                             double y_position)
{
}

void mouse_events_refiner::enter_function(GLFWwindow* window, int entered) { }

void mouse_events_refiner::scroll_function(GLFWwindow* window,
                                           double x_offset,
                                           double y_offset)
{
}

void mouse_events_refiner::drop_function(GLFWwindow* window,
                                         int path_count,
                                         const char** paths)
{
}
