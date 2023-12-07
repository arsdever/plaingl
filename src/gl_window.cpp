/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <glm/ext.hpp>
#include <prof/profiler.hpp>

#include "gl_window.hpp"

#include "asset_manager.hpp"
#include "camera.hpp"
#include "game_object.hpp"
#include "gizmo_object.hpp"
#include "logging.hpp"
#include "scene.hpp"
#include "shader.hpp"

namespace
{
static inline logger log() { return get_logger("window"); }
static inline void gl_debug_output(GLenum source,
                                   GLenum type,
                                   unsigned int id,
                                   GLenum severity,
                                   GLsizei length,
                                   const char* message,
                                   const void* userParam)
{
    // SOURCE: https://learnopengl.com/In-Practice/Debugging
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    log()->error("---------------");
    log()->error("Debug message ({}): {}", id, message);

    switch (source)
    {
    case GL_DEBUG_SOURCE_API: log()->error("Source: API"); break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        log()->error("Source: Window System");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        log()->error("Source: Shader Compiler");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        log()->error("Source: Third Party");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        log()->error("Source: Application");
        break;
    case GL_DEBUG_SOURCE_OTHER: log()->error("Source: Other"); break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR: log()->error("Type: Error"); break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        log()->error("Type: Deprecated Behaviour");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        log()->error("Type: Undefined Behaviour");
        break;
    case GL_DEBUG_TYPE_PORTABILITY: log()->error("Type: Portability"); break;
    case GL_DEBUG_TYPE_PERFORMANCE: log()->error("Type: Performance"); break;
    case GL_DEBUG_TYPE_MARKER: log()->error("Type: Marker"); break;
    case GL_DEBUG_TYPE_PUSH_GROUP: log()->error("Type: Push Group"); break;
    case GL_DEBUG_TYPE_POP_GROUP: log()->error("Type: Pop Group"); break;
    case GL_DEBUG_TYPE_OTHER: log()->error("Type: Other"); break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH: log()->error("Severity: high"); break;
    case GL_DEBUG_SEVERITY_MEDIUM: log()->error("Severity: medium"); break;
    case GL_DEBUG_SEVERITY_LOW: log()->error("Severity: low"); break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        log()->error("Severity: notification");
        break;
    }
}
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
        glDebugMessageCallback(gl_debug_output, nullptr);
        glDebugMessageControl(
            GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    configure_fps_text();
    configure_object_index_mapping();

    _last_frame_time = std::chrono::steady_clock::now();

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
    draw();
    on_custom_draw();

    glfwSwapBuffers(_window);
    glfwPollEvents();
}

void gl_window::draw()
{
    auto p = prof::profile(__FUNCTION__);
    auto now = std::chrono::steady_clock::now();
    auto diff = now - _last_frame_time;
    _last_frame_time = std::move(now);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    if (_index_rendering)
    {
        _object_index_map_shader.use();
        unsigned id = 0;
        for (auto object : scene::get_active_scene()->objects())
        {
            glm::mat4 model = object->get_transform().get_matrix();
            glm::mat4 mvp = _view_camera->vp_matrix() * model;
            glUniformMatrix4fv(_object_index_map_mvp_location,
                               1,
                               GL_FALSE,
                               glm::value_ptr(mvp));
            glUniform1ui(_object_index_map_id_location, ++id);
            object->get_mesh()->render();
        }
        shader_program::unuse();
    }
    else
    {
        if (scene::get_active_scene())
        {
            for (auto* obj : scene::get_active_scene()->objects())
            {
                obj->update();
            }
        }
    }

    // draw fps counter
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (scene::get_active_scene())
    {
        for (auto* obj : scene::get_active_scene()->gizmo_objects())
        {
            obj->update();
        }
    }

    glm::mat4 ortho = glm::ortho<float>(
        0.0f, static_cast<float>(width()), 0.0f, static_cast<float>(height()));
    _fps_text.get_shader()->set_uniform("projection", std::make_tuple(ortho));

    _fps_text.set_text(fmt::format(
        "{:#6.6} ms\nhandle {:#x}",
        std::chrono::duration_cast<std::chrono::duration<double>>(diff)
                .count() *
            1000,
        reinterpret_cast<unsigned long long>(this)));
    _fps_text.render();
    glDisable(GL_BLEND);
}

void gl_window::set_camera(camera* view_camera)
{
    _view_camera = view_camera;
    _view_camera->set_render_size(width(), height());
}

void gl_window::toggle_indexing() { _index_rendering = !_index_rendering; }

void gl_window::set_mouse_events_refiner(
    mouse_events_refiner* mouse_events_refiner_)
{
    _mouse_events = mouse_events_refiner_;
    setup_mouse_callbacks();
}

mouse_events_refiner* gl_window::mouse_events() const { return _mouse_events; }

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

void gl_window::configure_fps_text()
{
    font fps_text_font;
    fps_text_font.load("font.ttf", 12);

    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(width()), 0.0f, static_cast<float>(height()));
    _fps_text.init();
    _fps_text.set_position({ 5.0f, height() - 15.0f });
    _fps_text.set_color({ 1.0f, 1.0f, 1.0f });
    _fps_text.set_font(std::move(fps_text_font));
    _fps_text.set_scale(1);
    if (!asset_manager::default_asset_manager()->get_shader("text"))
    {
        asset_manager::default_asset_manager()->load_asset("text.shader");
    }
    _fps_text.set_shader(
        asset_manager::default_asset_manager()->get_shader("text"));
    _fps_text.get_shader()->set_uniform("projection",
                                        std::make_tuple(projection));

    on_window_resized += [ this ](auto... args) {
        _fps_text.set_position({ 5.0f, height() - 15.0f });
    };
}

void gl_window::configure_object_index_mapping()
{
    glfwMakeContextCurrent(_window);
    glGenFramebuffers(1, &_object_index_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _object_index_fbo);
    _object_index_map = new texture(width(), height());
    _object_index_map->init();
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT1,
                           GL_TEXTURE_2D,
                           _object_index_map->id(),
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

    _object_index_map_shader.init();
    _object_index_map_shader.add_shader("object_indexing.vert");
    _object_index_map_shader.add_shader("object_indexing.frag");
    _object_index_map_shader.link();
    _object_index_map_shader.use();
    _object_index_map_mvp_location =
        glGetUniformLocation(_object_index_map_shader.id(), "mvp_matrix");
    _object_index_map_id_location =
        glGetUniformLocation(_object_index_map_shader.id(), "object_index");
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
    _object_index_map_shader.use();
    unsigned id = 0;
    for (auto object : scene::get_active_scene()->objects())
    {
        glm::mat4 model = object->get_transform().get_matrix();
        glm::mat4 mvp = _view_camera->vp_matrix() * model;
        glUniformMatrix4fv(
            _object_index_map_mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform1ui(_object_index_map_id_location, ++id);
        object->get_mesh()->render();
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
