#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "mouse_events_refiner.hpp"

#include "logging.hpp"

// TODO: some logic may not work in case of simultaneous multiple button actions
// TODO: implement double click actions

namespace
{
logger log() { return get_logger("mouse_events"); }
} // namespace

mouse_events_refiner::mouse_events_refiner() = default;

float mouse_events_refiner::get_drag_distance() const
{
    return _drag_distance.value_or(_default_drag_distance);
}

void mouse_events_refiner::set_drag_distance(std::optional<float> distance)
{
    _drag_distance = distance;
}

void mouse_events_refiner::set_default_drag_distance(float distance)
{
    _default_drag_distance = distance;
}

void mouse_events_refiner::button_function(GLFWwindow* window,
                                           int button,
                                           int action,
                                           int mods)
{
    auto params = get_default_event_parameters(window);
    log()->trace("Mouse event on window {} | button {} | action {} | mods {}",
                 reinterpret_cast<unsigned long long>(window),
                 button,
                 action,
                 mods);

    if (action == GLFW_PRESS)
    {
        _buttons |= 1 << button;
        params._buttons_state = _buttons;
        _press_position = params._position;
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            press(params);
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            right_press(params);
            break;
        }
        case GLFW_MOUSE_BUTTON_MIDDLE:
        {
            middle_press(params);
            break;
        }
        default: break;
        }
    }
    else
    {
        _buttons &= ~(1 << button);
        params._buttons_state = _buttons;
        release(params);
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            release(params);
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            right_release(params);
            break;
        }
        case GLFW_MOUSE_BUTTON_MIDDLE:
        {
            middle_release(params);
            break;
        }
        default: break;
        }

        if (_is_drag)
        {
            drag_drop_end(params);
        }
        else
        {
            click(params);
        }
    }
}

void mouse_events_refiner::position_function(GLFWwindow* window,
                                             double x_position,
                                             double y_position)
{
    auto params = get_default_event_parameters(window);
    double distance = glm::distance(_press_position, params._position);
    move(params);

    // TODO: specify drag distance
    if (_buttons && distance > get_drag_distance())
    {
        if (_is_drag)
        {
            drag_drop_move(params);
        }
        else
        {
            drag_drop_start(params);
        }
        _is_drag = true;
    }
    else
    {
        _is_drag = false;
    }

    _mouse_position = params._position;
}

void mouse_events_refiner::enter_function(GLFWwindow* window, int entered)
{
    auto fptr = entered ? enter : leave;
    fptr(get_default_event_parameters(window));
}

void mouse_events_refiner::scroll_function(GLFWwindow* window,
                                           double x_offset,
                                           double y_offset)
{
    auto params = get_default_event_parameters(window);
    params._delta = { x_offset, y_offset };
    scroll(params);
}

void mouse_events_refiner::drop_function(GLFWwindow* window,
                                         int path_count,
                                         const char** paths)
{
    // TODO: implement
}

mouse_events_refiner::mouse_event_params
mouse_events_refiner::get_default_event_parameters(GLFWwindow* window) const
{
    mouse_event_params params;
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    params._window = static_cast<window*>(glfwGetWindowUserPointer(window));
    params._position = { x, y };
    params._old_position = _mouse_position;
    return params;
}

float mouse_events_refiner::_default_drag_distance = 5.0f;
