#pragma once

namespace core
{
class window;
}

class console;

class application
{
public:
    application();
    ~application();
    int run();

private:
    void setup_console();

    void update_windows();
    void process_console_commands();

    void register_components();
    void load_assets();

private:
    std::vector<std::shared_ptr<core::window>> _windows;
    std::shared_ptr<console> _console { nullptr };
};
