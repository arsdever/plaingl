/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <prof/profiler.hpp>

#include "gl_window.hpp"

#include "asset_manager.hpp"
#include "camera.hpp"
#include "components/mesh_component.hpp"
#include "game_object.hpp"
#include "gizmo_drawer.hpp"
#include "gizmo_object.hpp"
#include "gl_error_handler.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "shader.hpp"

namespace
{
static inline logger log() { return get_logger("window"); }
} // namespace

void gl_window::init()
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
        glfwCreateWindow(width(),
                         height(),
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

    glViewport(0, 0, width(), height());

    glfwSetFramebufferSizeCallback(_window,
                                   [](GLFWwindow* window, int w, int h)
    {
        gl_window* _this =
            static_cast<gl_window*>(glfwGetWindowUserPointer(window));
        _this->_width = w;
        _this->_height = h;
        _this->_view_camera->set_render_size(w, h);
    });

    glfwSetMouseButtonCallback(
        _window,
        [](GLFWwindow* window, int button, int action, int mods)
    {
        gl_window* _this =
            static_cast<gl_window*>(glfwGetWindowUserPointer(window));

        if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE)
        {
            if (_this->on_mouse_clicked.has_listeners())
            {
                double xpos;
                double ypos;
                glfwGetCursorPos(window, &xpos, &ypos);
                game_object* object =
                    _this->find_game_object_at_position(xpos, ypos);
                _this->on_mouse_clicked(object);
            }
        }
    });

    glfwSetFramebufferSizeCallback(_window,
                                   [](GLFWwindow* window, int w, int h)
    {
        gl_window* _this =
            static_cast<gl_window*>(glfwGetWindowUserPointer(window));
        _this->_width = w;
        _this->_height = h;
        _this->on_window_resized(_this, w, h);
        if (_this->_view_camera)
        {
            _this->_view_camera->set_render_size(w, h);
        }
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

    _state = state::initialized;
}

void gl_window::set_active() { glfwMakeContextCurrent(_window); }

size_t gl_window::width() const { return _width; }

size_t gl_window::height() const { return _height; }

void gl_window::resize(size_t width, size_t height)
{
    if (_state != state::initialized)
    {
        _width = width;
        _height = height;
    }
    else
    {
        glfwSetWindowSize(_window, width, height);
    }
}

glm::vec<2, size_t> gl_window::position() const
{
    int xpos = 0;
    int ypos = 0;
    glfwGetWindowPos(_window, &xpos, &ypos);
    return { xpos, ypos };
}

void gl_window::move(size_t x, size_t y) { glfwSetWindowPos(_window, x, y); }

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
        on_window_closed(this);
        return;
    }

    set_active();
    glViewport(0, 0, width(), height());
    _view_camera->set_active();
    if (auto* s = scene::get_active_scene())
    {
        for (auto* obj : s->objects())
        {
            obj->update();
        }
    }
    draw();
    on_custom_draw();

    glfwSwapBuffers(_window);
    glfwPollEvents();
}

void gl_window::draw()
{
    auto p = prof::profile(__FUNCTION__);
    glEnable(GL_MULTISAMPLE);
    _view_camera->render();

    if (auto* s = scene::get_active_scene())
    {
        for (auto* obj : s->objects())
        {
            gizmo_drawer::instance()->get_shader().set_uniform(
                "model_matrix",
                std::make_tuple(obj->get_transform().get_matrix()));
            obj->draw_gizmos();
        }
    }
}

void gl_window::set_camera(camera* view_camera)
{
    _view_camera = view_camera;
    _view_camera->set_render_size(width(), height());
}

void gl_window::toggle_indexing() { _index_rendering = !_index_rendering; }

void gl_window::set_draw_gizmos(bool value) { _should_draw_gizmos = value; }

void gl_window::set_mouse_events_refiner(
    mouse_events_refiner* mouse_events_refiner_)
{
    _mouse_events = mouse_events_refiner_;
    setup_mouse_callbacks();
}

mouse_events_refiner* gl_window::mouse_events() const { return _mouse_events; }

gl_window* gl_window::get_main_window() { return _main_window; }

void gl_window::setup_mouse_callbacks()
{
    glfwSetMouseButtonCallback(
        _window,
        [](GLFWwindow* window, int button, int action, int mods)
    {
        gl_window* _this =
            static_cast<gl_window*>(glfwGetWindowUserPointer(window));
        auto* refiner = _this->_mouse_events;
        refiner->button_function(window, button, action, mods);
    });
    glfwSetCursorPosCallback(
        _window,
        [](GLFWwindow* window, double x_position, double y_position)
    {
        gl_window* _this =

            static_cast<gl_window*>(glfwGetWindowUserPointer(window));
        auto* refiner = _this->_mouse_events;
        refiner->position_function(window, x_position, y_position);
    });
    glfwSetCursorEnterCallback(_window,
                               [](GLFWwindow* window, int entered)
    {
        gl_window* _this =
            static_cast<gl_window*>(glfwGetWindowUserPointer(window));
        auto* refiner = _this->_mouse_events;
        refiner->enter_function(window, entered);
    });

    glfwSetScrollCallback(
        _window,
        [](GLFWwindow* window, double x_offset, double y_offset)
    {
        gl_window* _this =
            static_cast<gl_window*>(glfwGetWindowUserPointer(window));
        auto* refiner = _this->_mouse_events;
        refiner->scroll_function(window, x_offset, y_offset);
    });
    glfwSetDropCallback(
        _window,
        [](GLFWwindow* window, int path_count, const char** paths)
    {
        gl_window* _this =
            static_cast<gl_window*>(glfwGetWindowUserPointer(window));
        auto* refiner = _this->_mouse_events;
        refiner->drop_function(window, path_count, paths);
    });
}

void gl_window::configure_object_index_mapping()
{
    glfwMakeContextCurrent(_window);
    glGenFramebuffers(1, &_object_index_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _object_index_fbo);
    glGenTextures(1, &_object_index_map);
    glBindTexture(GL_TEXTURE_2D, _object_index_map);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB32UI,
                 _width,
                 _height,
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
                 width(),
                 height(),
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

game_object* gl_window::find_game_object_at_position(double x, double y)
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

gl_window* gl_window::_main_window = nullptr;
