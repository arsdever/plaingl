#include <GLFW/glfw3.h>
#include <prof/profiler.hpp>

#include "application.hpp"

#include "application/application_commands.hpp"
#include "core/command_dispatcher.hpp"
#include "core/game_clock.hpp"
#include "core/window.hpp"
#include "logging.hpp"
#include "project/game_object.hpp"
#include "project/project_commands.hpp"
#include "project/scene.hpp"
#include "tools/console/console.hpp"

namespace
{
logger log() { return get_logger("app"); }
} // namespace

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

    setup_console();
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

void application::setup_console()
{
    cmd_show_mesh::open_window_requested +=
        [ this ](std::shared_ptr<core::window> w)
    {
        _windows.push_back(w);
        w->get_events()->close += [ this ](auto ce)
        { std::erase(_windows, ce.get_sender()->shared_from_this()); };
    };
    cmd_show_texture::open_window_requested +=
        [ this ](std::shared_ptr<core::window> w)
    {
        _windows.push_back(w);
        w->get_events()->close += [ this ](auto ce)
        { std::erase(_windows, ce.get_sender()->shared_from_this()); };
    };

    _console = std::make_unique<console>();
    _console->register_for_logs();
    _console->register_command<cmd_show_texture, int>("show.texture");
    _console->register_command<cmd_show_mesh, int>("show.mesh");
    _console->register_command<cmd_list_textures>("list.textures");
    _console->register_command<project::cmd_create_game_object>(
        "create.gameobject");
    _console->register_command<project::cmd_print_selected_object>(
        "show.selected");
    _console->register_command<project::cmd_rename_object, std::string>(
        "rename");
    _console->register_command<project::cmd_select_object, size_t>("select");
    _console->register_command<project::cmd_list_objects>("list.objects");
    _console->enable_autoactivation();
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
