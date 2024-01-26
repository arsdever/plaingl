#pragma once

#include <memory>
#include <string_view>

#include <glm/fwd.hpp>

#include "event.hpp"
#include "experimental/window_events.hpp"

namespace experimental
{

class window : public std::enable_shared_from_this<window>
{
public:
    window();
    ~window();

    void init();
    void activate();

    void set_title(std::string_view title);
    std::string get_title() const;

    size_t get_width() const;
    size_t get_height() const;
    glm::uvec2 get_size() const;
    void resize(size_t width, size_t height);

    glm::uvec2 get_position() const;
    void set_position(size_t x, size_t y);
    void move(size_t dx, size_t dy);

    void update();

    void set_can_grab(bool flag = true);
    bool get_can_grab() const;
    bool get_has_grab() const;

    std::shared_ptr<window_events> get_events() const;

    event<void(std::shared_ptr<window>)> on_closed;
    event<void(std::shared_ptr<window>, size_t w, size_t h)> on_resized;
    event<void(std::shared_ptr<window>)> on_draw_contents;

    static std::shared_ptr<window> get_main_window();

private:
    void setup_gl_debug_messages() const;
    void setup_mouse_callbacks();
    void configure_input_system();
    void key_callback(int key, int scancode, int action, int mods);

private:
    struct window_private_data;
    std::unique_ptr<window_private_data> _private_data { nullptr };

    static std::shared_ptr<window> _main_window;
};

} // namespace experimental
