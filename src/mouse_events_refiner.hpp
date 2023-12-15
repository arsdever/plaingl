#pragma once

#include <optional>

#include <glm/vec2.hpp>

#include "event.hpp"

struct GLFWwindow;
class window;

class mouse_events_refiner
{
public:
    struct mouse_event_params
    {
        window* _window;
        glm::vec2 _position;
        glm::vec2 _old_position;
        glm::vec2 _delta;
        unsigned _buttons_state;
    };

public:
    mouse_events_refiner();

    float get_drag_distance() const;
    void set_drag_distance(std::optional<float> distance);
    static void set_default_drag_distance(float distance);

    void button_function(GLFWwindow* window, int button, int action, int mods);
    void
    position_function(GLFWwindow* window, double x_position, double y_position);
    void enter_function(GLFWwindow* window, int entered);
    void scroll_function(GLFWwindow* window, double x_offset, double y_offset);
    void drop_function(GLFWwindow* window, int path_count, const char** paths);

    event<void(mouse_event_params)> press;
    event<void(mouse_event_params)> right_press;
    event<void(mouse_event_params)> middle_press;
    event<void(mouse_event_params)> click;
    event<void(mouse_event_params)> double_click;
    event<void(mouse_event_params)> right_click;
    event<void(mouse_event_params)> double_right_click;
    event<void(mouse_event_params)> middle_click;
    event<void(mouse_event_params)> release;
    event<void(mouse_event_params)> right_release;
    event<void(mouse_event_params)> middle_release;
    event<void(mouse_event_params)> move;
    event<void(mouse_event_params)> drag_drop_start;
    event<void(mouse_event_params)> drag_drop_move;
    event<void(mouse_event_params)> drag_drop_end;
    event<void(mouse_event_params)> enter;
    event<void(mouse_event_params)> leave;
    event<void(mouse_event_params)> scroll;

private:
    mouse_event_params get_default_event_parameters(GLFWwindow* params) const;

private:
    unsigned _buttons = 0;
    glm::vec2 _mouse_position;
    glm::vec2 _press_position;
    bool _is_drag = false;
    std::optional<float> _drag_distance;

    static float _default_drag_distance;
};
