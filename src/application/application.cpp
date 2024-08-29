#include <GLFW/glfw3.h>
#include <prof/profiler.hpp>

#include "application.hpp"

#include "core/command_dispatcher.hpp"
#include "core/game_clock.hpp"
#include "core/window.hpp"
#include "project/game_object.hpp"
#include "project/scene.hpp"
#include "tools/console/console.hpp"

application::application()
    : _console(std::make_shared<console>())
{
    glfwInit();
    auto main_window = std::make_shared<core::window>();
    main_window->resize(800, 600);
    main_window->set_title("Gamify");
    main_window->init();
    main_window->get_events()->close += [ this ](auto ce)
    { std::erase(_windows, ce.get_sender()->shared_from_this()); };
    _windows.push_back(main_window);
}

application::~application() { glfwTerminate(); }

int application::run()
{
    while (!_windows.empty())
    {
        update_windows();
        process_console_commands();
        game_clock::frame();
    }
    return 0;
}

void application::update_windows()
{
    auto p = prof::profile_frame(__FUNCTION__);
    if (scene::get_active_scene())
    {
        scene::get_active_scene()->visit_root_objects([](auto obj)
        { obj->update(); });
    }

    for (int i = 0; i < _windows.size(); ++i)
    {
        auto window = _windows[ i ];
        window->update();
    }
}

void application::process_console_commands()
{
    _console->processor().execute_all();
}
