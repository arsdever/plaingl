#pragma once

#include "core/command.hpp"
#include "project/uid.hpp"

class game_object;
class object;

namespace project
{
class cmd_create_game_object : public core::command
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

class cmd_rename_object : public core::command
{
public:
    cmd_rename_object(std::string name);
    void execute() override;

private:
    std::string _name;
};

class cmd_select_object : public core::command
{
public:
    cmd_select_object(uid obj_id);
    void execute() override;

private:
    uid _object_id;
};

class cmd_print_selected_object : public core::command
{
public:
    void execute() override;
};
} // namespace project
