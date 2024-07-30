#pragma once

#include "core/command.hpp"
#include "project/uid.hpp"

class game_object;
class object;

namespace project
{
class cmd_create_game_object : public core::command<>
{
public:
    void execute() override;

    std::shared_ptr<game_object> get_object() const;

private:
    std::shared_ptr<game_object> _obj;
};

// class cmd_destroy_game_object : public core::command
// {
// public:
//     cmd_destroy_game_object(std::shared_ptr<game_object> obj);
//     void execute() override;

// private:
//     std::shared_ptr<game_object> _obj;
// };

class cmd_rename_object : public core::command<std::string>
{
public:
    using command::command;
    void execute() override;
};

class cmd_select_object : public core::command<size_t>
{
public:
    using command::command;
    void execute() override;
};

class cmd_print_selected_object : public core::command<>
{
public:
    void execute() override;
};

class cmd_list_objects : public core::command<>
{
public:
    void execute() override;
};

std::shared_ptr<object> selected_object();
} // namespace project
