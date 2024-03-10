#include <thread>

#include <GLFW/glfw3.h>

#include "experimental/console/commands.hpp"
#include "experimental/console/commands/command.hpp"
#include "experimental/window.hpp"
#include "experimental/window_manager.hpp"
#include "logging.hpp"
#include "thread.hpp"

class application
{
public:
    void init()
    {
        glfwSetErrorCallback([](int ec, const char* msg)
        { log()->error("GLFW ({}): {}", ec, msg); });
        glfwInit();
        setupRenderThread();
        setupCommandCenter();
    }

    void setupCommandCenter() { }

    void run()
    {
        log()->info("The console is ready and waiting for inputs...");
        int ch;
        std::string input;
        input.reserve(64);
        while (!application_exits)
        {
            auto command = command_center::parse_command();
            command->execute();
        }

        log()->info("Console shuts down");
    }

    void setupRenderThread()
    {
        render_thread =
            std::thread { [ this ]() { experimental::window_manager::run(); } };
        render_thread.detach();
        set_thread_name(render_thread, "render_thread");
    }

    static logger log() { return get_logger("app"); }

private:
    std::thread render_thread;
    std::atomic_bool application_exits;
    std::vector<std::shared_ptr<experimental::window>> windows;
    std::mutex windows_mutex;
};

int main()
{
    application app;
    app.init();
    app.run();
}
