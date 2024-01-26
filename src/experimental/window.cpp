/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <glm/ext.hpp>
#include <prof/profiler.hpp>

#include "experimental/window.hpp"

#include "gl_error_handler.hpp"
#include "input_system.hpp"
#include "logging.hpp"

namespace
{
logger log() { return get_logger("window"); }

experimental::mouse_event::modifiers
glfw_keyboard_modifiers_to_mouse_event_modifiers(int mods)
{
    experimental::mouse_event::modifiers modifiers;
    modifiers = static_cast<experimental::mouse_event::modifiers>(mods);
    return modifiers;
}

} // namespace

namespace experimental
{

struct window::window_private_data
{
    GLFWwindow* _glfw_window_handle { nullptr };
    glm::vec<2, size_t> _size { 800, 600 };
    std::shared_ptr<window_events> _events { nullptr };
    std::string _title { "Window" };
    bool _is_main_window { false };
    bool _can_grab { false };
    bool _has_grab { false };
    struct
    {
        int _buttons;
        mouse_event::modifiers _mods;
    } _mouse_state;
};

window::window() { _private_data = std::make_unique<window_private_data>(); }

window::~window() = default;

void window::init()
{
    std::string title = _private_data->_title;
    if (!_main_window)
    {
        _main_window = shared_from_this();
        _private_data->_is_main_window = true;
    }

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _private_data->_glfw_window_handle = glfwCreateWindow(
        static_cast<int>(get_width()),
        static_cast<int>(get_height()),
        title.c_str(),
        nullptr,
        _private_data->_is_main_window
            ? nullptr
            : _main_window->_private_data->_glfw_window_handle);
    glfwSetWindowUserPointer(_main_window->_private_data->_glfw_window_handle,
                             this);
    if (_private_data->_glfw_window_handle == nullptr)
    {
        log()->error("Failed to create GLFW window");
        return;
    }

    activate();

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        log()->error("Failed to initialize GLAD");
        return;
    }

    glfwSetFramebufferSizeCallback(_private_data->_glfw_window_handle,
                                   [](GLFWwindow* wnd, int w, int h)
    {
        auto _this = static_cast<window*>(glfwGetWindowUserPointer(wnd));
        auto width = static_cast<size_t>(w);
        auto height = static_cast<size_t>(h);
        _this->_private_data->_size = { width, height };
        _this->on_resized(_this->shared_from_this(), width, height);
    });

    setup_gl_debug_messages();
    configure_input_system();
}

void window::activate()
{
    glfwMakeContextCurrent(_private_data->_glfw_window_handle);
}

void window::set_title(std::string_view title)
{
    _private_data->_title = title;
    if (_private_data->_glfw_window_handle)
    {
        glfwSetWindowTitle(_private_data->_glfw_window_handle, title.data());
    }
}

std::string window::get_title() const
{
    return std::string(_private_data->_title);
}

size_t window::get_width() const { return _private_data->_size.x; }

size_t window::get_height() const { return _private_data->_size.y; }

glm::uvec2 window::get_size() const { return _private_data->_size; }

void window::resize(size_t width, size_t height)
{
    _private_data->_size = { width, height };
    if (_private_data->_glfw_window_handle)
    {
        glfwSetWindowSize(_private_data->_glfw_window_handle,
                          static_cast<int>(width),
                          static_cast<int>(height));
    }
}

glm::uvec2 window::get_position() const
{
    int xpos = 0;
    int ypos = 0;
    glfwGetWindowPos(_private_data->_glfw_window_handle, &xpos, &ypos);
    return { xpos, ypos };
}

void window::move(size_t x, size_t y)
{
    glfwSetWindowPos(_private_data->_glfw_window_handle,
                     static_cast<int>(x),
                     static_cast<int>(y));
}

void window::update()
{
    if (_private_data->_glfw_window_handle == nullptr)
    {
        return;
    }

    if (glfwWindowShouldClose(_private_data->_glfw_window_handle))
    {
        glfwDestroyWindow(_private_data->_glfw_window_handle);
        _private_data->_glfw_window_handle = nullptr;
        on_closed(shared_from_this());
        return;
    }

    if (input_system::is_key_down(345) && input_system::is_key_down(346))
    {
        glfwSetInputMode(_private_data->_glfw_window_handle,
                         GLFW_CURSOR,
                         GLFW_CURSOR_NORMAL);
        _private_data->_has_grab = false;
    }

    activate();

    on_draw_contents(shared_from_this());

    glfwSwapBuffers(_private_data->_glfw_window_handle);
    glfwPollEvents();
}

void window::set_can_grab(bool flag) { _private_data->_can_grab = flag; }

bool window::get_can_grab() const { return _private_data->_can_grab; }

bool window::get_has_grab() const { return _private_data->_has_grab; }

std::shared_ptr<window_events> window::get_events() const
{
    return _private_data->_events;
}

std::shared_ptr<window> window::get_main_window() { return _main_window; }

void window::setup_gl_debug_messages() const
{
    // configure gl debug output
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_error_handler, nullptr);
    glDebugMessageControl(
        GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}

void window::setup_mouse_callbacks()
{
    glfwSetMouseButtonCallback(
        _private_data->_glfw_window_handle,
        [](GLFWwindow* wnd, int button, int action, int mods)
    {
        auto _this = static_cast<window*>(glfwGetWindowUserPointer(wnd));
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            _this->_private_data->_has_grab = true;
        }

        std::shared_ptr<window_events> events = _this->get_events();
        // events->mouse_click();
        // events->button_function(wnd, button, action, mods);
    });
    glfwSetCursorPosCallback(
        _private_data->_glfw_window_handle,
        [](GLFWwindow* wnd, double x_position, double y_position)
    {
        auto _this = static_cast<window*>(glfwGetWindowUserPointer(wnd));
        std::shared_ptr<window_events> events = _this->get_events();
        events->mouse_move(
            mouse_event(window_event::type::MouseMove,
                        { x_position, y_position },
                        { x_position, y_position },
                        { x_position, y_position },
                        0,
                        _this->_private_data->_mouse_state._buttons,
                        _this->_private_data->_mouse_state._mods));
    });
    glfwSetCursorEnterCallback(_private_data->_glfw_window_handle,
                               [](GLFWwindow* wnd, int entered)
    {
        auto _this = static_cast<window*>(glfwGetWindowUserPointer(wnd));
        std::shared_ptr<window_events> events = _this->get_events();
        // events->enter_function(wnd, entered);
    });

    glfwSetScrollCallback(_private_data->_glfw_window_handle,
                          [](GLFWwindow* wnd, double x_offset, double y_offset)
    {
        auto _this = static_cast<window*>(glfwGetWindowUserPointer(wnd));
        std::shared_ptr<window_events> events = _this->get_events();
        // events->scroll_function(wnd, x_offset, y_offset);
    });
    glfwSetDropCallback(_private_data->_glfw_window_handle,
                        [](GLFWwindow* wnd, int path_count, const char** paths)
    {
        auto _this = static_cast<window*>(glfwGetWindowUserPointer(wnd));
        std::shared_ptr<window_events> events = _this->get_events();
        // events->drop_function(wnd, path_count, paths);
    });
}

void window::configure_input_system()
{
    _private_data->_events = std::make_shared<window_events>();
    setup_mouse_callbacks();

    glfwSetKeyCallback(
        _private_data->_glfw_window_handle,
        [](GLFWwindow* wnd, int key, int scancode, int action, int mods)
    {
        auto _this = static_cast<window*>(glfwGetWindowUserPointer(wnd));
        _this->key_callback(key, scancode, action, mods);
    });
}

void window::key_callback(int key, int scancode, int action, int mods)
{
    (void)scancode;
    _private_data->_mouse_state._mods =
        glfw_keyboard_modifiers_to_mouse_event_modifiers(mods);
    input_system::set_key_down(key, action != GLFW_RELEASE);
}

std::shared_ptr<window> window::_main_window = nullptr;

} // namespace experimental
