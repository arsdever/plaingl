#pragma once

#include <chrono>
#include <unordered_map>

#include "text.hpp"

struct GLFWwindow;

class gl_window
{
    enum class state
    {
        uninitialized,
        initialized,
        closed,
    };

public:
    void init();
    void set_active();

    size_t width() const;
    size_t height() const;

    void update();
    void draw();

private:
    void configure_fps_text();

private:
    GLFWwindow* _window = nullptr;
    state _state = state::uninitialized;
    size_t _width = 800;
    size_t _height = 600;
    std::chrono::steady_clock::time_point _last_frame_time;
    text _fps_text;
    static std::unordered_map<GLFWwindow*, gl_window&> _window_mapping;
};
