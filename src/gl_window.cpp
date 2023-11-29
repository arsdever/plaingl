/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include "gl_window.hpp"

#include "logging.hpp"

namespace
{
static inline logger log() { return get_logger("window"); }
} // namespace

void gl_window::init()
{
    _width = 800;
    _height = 600;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    _window = glfwCreateWindow(width(), height(), "LearnOpenGL", NULL, NULL);
    if (_window == nullptr)
    {
        log()->error("Failed to create GLFW window");
        return;
    }

    auto [ existing_iterator, success ] =
        _window_mapping.try_emplace(_window, *this);
    if (!success)
    {
        _window_mapping.erase(existing_iterator);
        _window_mapping.emplace(_window, *this);
    }

    set_active();

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        log()->error("Failed to initialize GLAD");
        return;
    }

    glViewport(0, 0, width(), height());
    glfwSetFramebufferSizeCallback(_window,
                                   [](GLFWwindow* window, int w, int h)
    {
        gl_window& _this = _window_mapping.at(window);
        _this._width = w;
        _this._width = h;
        glViewport(0, 0, _this.width(), _this.height());
    });

    _state = state::initialized;
}

void gl_window::set_active() { glfwMakeContextCurrent(_window); }

void gl_window::update()
{
    if (_state != state::initialized)
    {
        // TODO: notify that the window was closed
        return;
    }

    if (glfwWindowShouldClose(_window))
    {
        _state = state::closed;
        glfwDestroyWindow(_window);
        _window = nullptr;
        return;
    }

    set_active();
    draw();
    ++_fps_counter;

    glfwSwapBuffers(_window);
}

void gl_window::draw()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

size_t gl_window::width() const { return _width; }

size_t gl_window::height() const { return _height; }

std::unordered_map<GLFWwindow*, gl_window&> gl_window::_window_mapping;
