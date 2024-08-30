#pragma once

namespace core
{
class window;
}

class console;
class scene;

class application
{
public:
    application();
    ~application();

    int run();
    void shutdown();

private:
    void setup_console();

    void update_windows();
    void process_console_commands();

    void register_components();
    void load_assets();

    void render_game();

    void scene_loaded(std::shared_ptr<scene> sc);

private:
    std::vector<std::shared_ptr<core::window>> _windows;
    std::shared_ptr<console> _console { nullptr };
    bool _is_running;
};
