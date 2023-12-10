#pragma once

#include <chrono>
#include <functional>
#include <unordered_map>

#include "event.hpp"
#include "mouse_events_refiner.hpp"

struct GLFWwindow;
class camera;
class game_object;
class shader_program;

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

    void resize(size_t width, size_t height);

    void update();
    void draw();

    void set_camera(camera* view_camera);

    void toggle_indexing();

    event<void(game_object*)> on_mouse_clicked;
    event<void(gl_window*)> on_window_closed;
    event<void(gl_window*, size_t w, size_t h)> on_window_resized;
    // TODO: handling should be improved
    event<void()> on_custom_draw;

    void set_mouse_events_refiner(mouse_events_refiner* mouse_events_refiner_);
    mouse_events_refiner* mouse_events() const;

    game_object* find_game_object_at_position(double x, double y);

    static gl_window* get_main_window();

private:
    void setup_mouse_callbacks();
    void configure_object_index_mapping();

private:
    GLFWwindow* _window = nullptr;
    state _state = state::uninitialized;
    size_t _width = 800;
    size_t _height = 600;
    bool _is_main_window = false;
    camera* _view_camera = nullptr;
    mouse_events_refiner* _mouse_events;

    unsigned _object_index_map;
    unsigned _object_index_depth_map;
    unsigned _object_index_fbo;
    shader_program* _object_index_map_shader;
    bool _index_rendering = false;

    static gl_window* _main_window;
};
