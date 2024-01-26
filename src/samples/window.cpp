#include <memory>

#include <GLFW/glfw3.h>

#include "experimental/window.hpp"

#include "logging.hpp"

namespace
{
logger log() { return get_logger("window example"); }
} // namespace

int main(int argc, char** argv)
{
    glfwInit();
    std::vector<std::shared_ptr<experimental::window>> windows;
    std::shared_ptr<experimental::window> exp_window =
        std::make_shared<experimental::window>();
    exp_window->set_title("Hello experimental window");
    exp_window->init();

    exp_window->set_can_grab(true);

    exp_window->get_events()->mouse_move += [](auto me)
    {
        log()->info("Mouse position: {}x{}",
                    me.get_local_position().x,
                    me.get_local_position().y);
    };

    exp_window->get_events()->leave +=
        [](auto ee) { log()->info("Cursor left the window"); };

    exp_window->get_events()->enter += [](auto ee)
    {
        log()->info("Cursor entered the window at: {}x{}",
                    ee.get_local_position().x,
                    ee.get_local_position().y);
    };

    exp_window->get_events()->move += [](auto me)
    {
        log()->info("Window moved from: {}x{} to: {}x{}",
                    me.get_old_position().x,
                    me.get_old_position().y,
                    me.get_new_position().x,
                    me.get_new_position().y);
    };

    exp_window->get_events()->resize += [](auto re)
    {
        log()->info("Window resized from: {}x{} to: {}x{}",
                    re.get_old_size().x,
                    re.get_old_size().y,
                    re.get_new_size().x,
                    re.get_new_size().y);
    };

    exp_window->get_events()->mouse_release +=
        [](auto me) { log()->info("Mouse release: {}", me.get_button()); };

    exp_window->get_events()->mouse_press +=
        [](auto me) { log()->info("Mouse press: {}", me.get_button()); };

    exp_window->get_events()->mouse_click +=
        [](auto me) { log()->info("Mouse click: {}", me.get_button()); };

    exp_window->get_events()->mouse_double_click +=
        [](auto me) { log()->info("Mouse double click: {}", me.get_button()); };

    exp_window->get_events()->mouse_triple_click +=
        [](auto me) { log()->info("Mouse triple click: {}", me.get_button()); };

    exp_window->get_events()->mouse_scroll += [](auto we)
    { log()->info("Mouse scroll: {}x{}", we.get_delta().x, we.get_delta().y); };

    windows.push_back(exp_window);

    while (!windows.empty())
    {
        for (int i = 0; i < windows.size(); ++i)
        {
            auto window = windows[ i ];
            window->update();
        }
    }

    return 0;
}
