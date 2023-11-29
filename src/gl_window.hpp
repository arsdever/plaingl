#pragma once

#include <unordered_map>

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
    GLFWwindow* _window = nullptr;
    state _state = state::uninitialized;
    size_t _width = 800;
    size_t _height = 600;
    size_t _fps_counter = 0;
    static std::unordered_map<GLFWwindow*, gl_window&> _window_mapping;
};
