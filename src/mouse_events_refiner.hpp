#pragma once

#include <glm/vec2.hpp>

#include "event.hpp"

struct GLFWwindow;
class gl_window;

class mouse_events_refiner
{
public:
    struct mouse_event_params
    {
        glm::vec2 position;
        unsigned buttons_state;
    };

public:
    mouse_events_refiner();

    void button_function(GLFWwindow* window, int button, int action, int mods);
    void position_function(GLFWwindow* window, double x_position, double y_position);
    void enter_function(GLFWwindow* window, int entered);
    void scroll_function(GLFWwindow* window, double x_offset, double y_offset);
    void drop_function(GLFWwindow* window, int path_count, const char** paths);

    event<void(mouse_event_params)> press;
    event<void(gl_window*, glm::vec2)> click;
    event<void(mouse_event_params)> double_click;
    event<void(mouse_event_params)> release;
};
