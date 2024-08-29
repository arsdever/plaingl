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
    void update_windows();
    void process_console_commands();

private:
    std::vector<std::shared_ptr<core::window>> _windows;
    std::shared_ptr<console> _console { nullptr };
};
