#include <prof/profiler.hpp>

#include "window.hpp"

#include "asset_manager.hpp"
#include "camera.hpp"
#include "components/mesh_component.hpp"
#include "game_object.hpp"
#include "gl_error_handler.hpp"
#include "input_system.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "viewport.hpp"

namespace
{
static inline logger log() { return get_logger("window"); }
} // namespace

void window::init()
{
    std::string title = "Window";
    if (!_main_window)
    {
        _main_window = this;
        _is_main_window = true;
        title = "Main window";
    }

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    _window =
        glfwCreateWindow(get_width(),
                         get_height(),
                         title.c_str(),
                         NULL,
                         _is_main_window ? nullptr : _main_window->_window);
    glfwSetWindowUserPointer(_window, this);
    if (_window == nullptr)
    {
        log()->error("Failed to create GLFW window");
        return;
    }

    set_active();

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        log()->error("Failed to initialize GLAD");
        return;
    }

    glViewport(0, 0, get_width(), get_height());

    glfwSetFramebufferSizeCallback(_window,
                                   [](GLFWwindow* wnd, int w, int h)
    {
        window* _this = static_cast<window*>(glfwGetWindowUserPointer(wnd));
        _this->_size = { static_cast<size_t>(w), static_cast<size_t>(h) };
        _this->update_layout();
        _this->on_window_resized(_this, w, h);
    });

    {
        // configure gl debug output
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(reinterpret_cast<GLDEBUGPROC>(gl_error_handler),
                               nullptr);
        glDebugMessageControl(
            GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    _object_index_map_shader = new shader_program;

    configure_object_index_mapping();
    configure_input_system();

    set_layout<layout_single>();
    update_layout();

    _state = state::initialized;
}

void window::set_active() { glfwMakeContextCurrent(_window); }

size_t window::get_width() const { return _size.x; }

size_t window::get_height() const { return _size.y; }

glm::vec<2, size_t> window::get_size() const { return _size; }

void window::resize(size_t width, size_t height)
{
    if (_state != state::initialized)
    {
        _size = { width, height };
    }
    else
    {
        glfwSetWindowSize(_window, width, height);
    }
    // update viewport layout
}

glm::vec<2, size_t> window::position() const
{
    int xpos = 0;
    int ypos = 0;
    glfwGetWindowPos(_window, &xpos, &ypos);
    return { xpos, ypos };
}

void window::move(size_t x, size_t y) { glfwSetWindowPos(_window, x, y); }

void window::update()
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
        on_window_closed(this);
        return;
    }

    if (input_system::is_key_down(345) && input_system::is_key_down(346))
    {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        _has_grab = false;
    }

    set_active();
    // TODO: consider using glScissor for viewport dependent cleaning
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _view_camera->set_active();
    if (auto* s = scene::get_active_scene())
    {
        for (auto* obj : s->objects())
        {
            obj->update();
        }
    }

    for (auto& vp : get_viewports())
    {
        vp->update();
    }

    glViewport(0, 0, get_width(), get_height());
    glfwSwapBuffers(_window);
    glfwPollEvents();
}

void window::toggle_indexing() { _index_rendering = !_index_rendering; }

bool window::has_grab() const { return _has_grab; }

void window::add_viewport(std::shared_ptr<viewport> vp)
{
    vp->set_window(this);
    _viewports.push_back(vp);
    _layout->calculate_layout(this);
}

std::vector<std::shared_ptr<viewport>> window::get_viewports() const
{
    return _viewports;
}

void window::remove_viewport(std::shared_ptr<viewport> vp)
{
    vp->set_window(nullptr);
    std::erase(_viewports, vp);
    _layout->calculate_layout(this);
}

std::shared_ptr<viewport> window::get_main_viewport()
{
    return _viewports[ 0 ];
}

void window::set_mouse_events_refiner(
    mouse_events_refiner* mouse_events_refiner_)
{
    _mouse_events = mouse_events_refiner_;
    setup_mouse_callbacks();
}

mouse_events_refiner* window::mouse_events() const { return _mouse_events; }

window* window::get_main_window() { return _main_window; }

void window::update_layout() { _layout->calculate_layout(this); }

void window::setup_mouse_callbacks()
{
    glfwSetMouseButtonCallback(
        _window,
        [](GLFWwindow* wnd, int button, int action, int mods)
    {
        window* _this = static_cast<window*>(glfwGetWindowUserPointer(wnd));
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            _this->_has_grab = true;
        }

        auto* refiner = _this->_mouse_events;
        refiner->button_function(wnd, button, action, mods);
    });
    glfwSetCursorPosCallback(
        _window,
        [](GLFWwindow* wnd, double x_position, double y_position)
    {
        window* _this =

            static_cast<window*>(glfwGetWindowUserPointer(wnd));
        auto* refiner = _this->_mouse_events;
        refiner->position_function(wnd, x_position, y_position);
    });
    glfwSetCursorEnterCallback(_window,
                               [](GLFWwindow* wnd, int entered)
    {
        window* _this = static_cast<window*>(glfwGetWindowUserPointer(wnd));
        auto* refiner = _this->_mouse_events;
        refiner->enter_function(wnd, entered);
    });

    glfwSetScrollCallback(_window,
                          [](GLFWwindow* wnd, double x_offset, double y_offset)
    {
        window* _this = static_cast<window*>(glfwGetWindowUserPointer(wnd));
        auto* refiner = _this->_mouse_events;
        refiner->scroll_function(wnd, x_offset, y_offset);
    });
    glfwSetDropCallback(_window,
                        [](GLFWwindow* wnd, int path_count, const char** paths)
    {
        window* _this = static_cast<window*>(glfwGetWindowUserPointer(wnd));
        auto* refiner = _this->_mouse_events;
        refiner->drop_function(wnd, path_count, paths);
    });
}

void window::configure_input_system()
{
    glfwSetKeyCallback(
        _window,
        [](GLFWwindow* wnd, int key, int scancode, int action, int mods)
    {
        window* _this = static_cast<window*>(glfwGetWindowUserPointer(wnd));
        _this->key_callback(key, scancode, action, mods);
    });
}

void window::key_callback(int key, int scancode, int action, int mods)
{
    input_system::set_key_down(key, action != GLFW_RELEASE);
}

void window::configure_object_index_mapping()
{
    glfwMakeContextCurrent(_window);
    glGenFramebuffers(1, &_object_index_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _object_index_fbo);
    glGenTextures(1, &_object_index_map);
    glBindTexture(GL_TEXTURE_2D, _object_index_map);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB32UI,
                 get_width(),
                 get_height(),
                 0,
                 GL_RGB_INTEGER,
                 GL_UNSIGNED_INT,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT1,
                           GL_TEXTURE_2D,
                           _object_index_map,
                           0);

    glGenTextures(1, &_object_index_depth_map);
    glBindTexture(GL_TEXTURE_2D, _object_index_depth_map);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT,
                 get_width(),
                 get_height(),
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D,
                           _object_index_depth_map,
                           0);
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        log()->info("Framebuffer error: {}", status);
        return;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _object_index_map_shader->init();
    _object_index_map_shader->add_shader("object_indexing.vert");
    _object_index_map_shader->add_shader("object_indexing.frag");
    _object_index_map_shader->link();
    _object_index_map_shader->use();
    shader_program::unuse();
}

game_object* window::find_game_object_at_position(double x, double y)
{
    set_active();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _object_index_fbo);
    GLenum buffers[] { GL_NONE, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, buffers);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    unsigned id = 0;
    for (auto object : scene::get_active_scene()->objects())
    {
        glm::mat4 model = object->get_transform().get_matrix();
        glm::mat4 mvp = _view_camera->vp_matrix() * model;
        _object_index_map_shader->set_uniform("mvp_matrix",
                                              std::make_tuple(mvp));
        _object_index_map_shader->set_uniform("object_id",
                                              std::make_tuple(++id));
        _object_index_map_shader->use();
        if (object->get_component<mesh_component>())
        {
            object->get_component<mesh_component>()->get_mesh()->render();
        }
    }
    shader_program::unuse();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, _object_index_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    struct pixel_info
    {
        unsigned id;
        unsigned reserved1;
        unsigned reserved2;
    } pixel_info;
    glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &pixel_info);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    if (pixel_info.id > 0)
    {
        return scene::get_active_scene()->objects()[ pixel_info.id - 1 ];
    }

    return nullptr;
}

window* window::_main_window = nullptr;

void window::layout_single::calculate_layout(window* wnd)
{
    if (wnd->_viewports.empty())
    {
        return;
    }

    wnd->_viewports[ 0 ]->set_visible();
    wnd->_viewports[ 0 ]->set_position(0, 0);
    wnd->_viewports[ 0 ]->set_size(wnd->get_size());
}
