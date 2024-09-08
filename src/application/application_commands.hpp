#pragma once

#include "core/core_fwd.hpp"

#include "common/event.hpp"
#include "core/command.hpp"

class cmd_show_profiler : public core::command<>
{
public:
    void execute() override;

    static event<void(std::shared_ptr<core::window>)> open_window_requested;
};

class cmd_show_mesh : public core::command<std::string>
{
public:
    using command::command;
    void execute() override;

    static event<void(std::shared_ptr<core::window>)> open_window_requested;
};

class cmd_show_texture : public core::command<std::string>
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
