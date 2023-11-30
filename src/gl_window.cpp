/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <glm/ext.hpp>

#include "gl_window.hpp"

#include "logging.hpp"
#include "shader.hpp"

namespace
{
static inline logger log() { return get_logger("window"); }
} // namespace

void gl_window::init()
{
    _width = 800;
    _height = 600;

    glfwWindowHint(GLFW_SAMPLES, 8);
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

    configure_fps_text();
    _last_frame_time = std::chrono::steady_clock::now();

    _state = state::initialized;
}

void gl_window::set_active() { glfwMakeContextCurrent(_window); }

size_t gl_window::width() const { return _width; }

size_t gl_window::height() const { return _height; }

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

    glfwSwapBuffers(_window);
}

void gl_window::draw()
{
    auto now = std::chrono::steady_clock::now();
    auto diff = now - _last_frame_time;
    _last_frame_time = std::move(now);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw fps counter
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    _fps_text.set_text(fmt::format(
        "{:#6.6} ms",
        std::chrono::duration_cast<std::chrono::duration<double>>(diff)
                .count() *
            1000));
    _fps_text.render();
    glDisable(GL_BLEND);
}

void gl_window::configure_fps_text()
{
    font fps_text_font;
    fps_text_font.load("font.ttf", 9);

    shader_program prog;
    prog.init();
    prog.add_shader("text.vert");
    prog.add_shader("text.frag");
    prog.link();

    prog.use();
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    unsigned int uniform_position =
        glGetUniformLocation(prog.id(), "projection");
    glUniformMatrix4fv(
        uniform_position, 1, GL_FALSE, glm::value_ptr(projection));
    prog.unuse();

    _fps_text.init();
    _fps_text.set_position({ 5.0f, height() - 10.0f });
    _fps_text.set_color({ 1.0f, 1.0f, 1.0f });
    _fps_text.set_font(std::move(fps_text_font));
    _fps_text.set_scale(1);
    _fps_text.set_shader(std::move(prog));
}

std::unordered_map<GLFWwindow*, gl_window&> gl_window::_window_mapping;
