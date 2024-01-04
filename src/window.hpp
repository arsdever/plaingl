#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>

#include "event.hpp"
#include "mouse_events_refiner.hpp"

struct GLFWwindow;
class camera;
class game_object;
class shader_program;
class viewport;

class window
{
private:
    enum class state
    {
        uninitialized,
        initialized,
        closed,
    };

public:
    struct layout
    {
        virtual ~layout() = default;
        virtual void calculate_layout(window*) = 0;
    };

    struct layout_single : layout
    {
        void calculate_layout(window*) override;
    };

public:
    void init();
    void set_active();

    size_t get_width() const;
    size_t get_height() const;
    glm::vec<2, size_t> get_size() const;
    void resize(size_t width, size_t height);

    glm::vec<2, size_t> position() const;
    void move(size_t x, size_t y);

    void update();

    void toggle_indexing();
    bool has_grab() const;

    void add_viewport(std::shared_ptr<viewport> vp);
    std::vector<std::shared_ptr<viewport>> get_viewports() const;
    void remove_viewport(std::shared_ptr<viewport> vp);
    std::shared_ptr<viewport> get_main_viewport();

    template <typename T>
        requires std::is_base_of_v<layout, T>
    void set_layout()
    {
        _layout = std::make_unique<T>();
        update_layout();
    }

    event<void(window*)> on_window_closed;
    event<void(window*, size_t w, size_t h)> on_window_resized;
    // TODO: handling should be improved
    event<void()> on_custom_draw;

    void set_mouse_events_refiner(mouse_events_refiner* mouse_events_refiner_);
    mouse_events_refiner* mouse_events() const;

    game_object* find_game_object_at_position(double x, double y);

    static window* get_main_window();

private:
    void update_layout();
    void setup_mouse_callbacks();
    void configure_object_index_mapping();
    void configure_input_system();

    static void key_callback(int key, int scancode, int action, int mods);

private:
    GLFWwindow* _window { nullptr };
    state _state = state::uninitialized;
    glm::vec<2, size_t> _size { 800, 600 };
    bool _is_main_window { false };
    camera* _view_camera { nullptr };
    std::unique_ptr<layout> _layout { nullptr };
    mouse_events_refiner* _mouse_events { nullptr };
    std::vector<std::shared_ptr<viewport>> _viewports;
    bool _has_grab = false;

    unsigned _object_index_map;
    unsigned _object_index_depth_map;
    unsigned _object_index_fbo;
    shader_program* _object_index_map_shader;
    bool _index_rendering { false };

    static window* _main_window;
};
