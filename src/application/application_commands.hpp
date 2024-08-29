#pragma once

#include "common/event.hpp"
#include "core/command.hpp"

namespace core
{
class window;
}

class cmd_show_mesh : public core::command<int>
{
public:
    using command::command;
    void execute() override;

    static event<void(std::shared_ptr<core::window>)> open_window_requested;
};

class cmd_show_texture : public core::command<int>
{
public:
    using command::command;
    void execute() override;

    static event<void(std::shared_ptr<core::window>)> open_window_requested;
};

class cmd_list_textures : public core::command<>
{
public:
    void execute() override;
};
