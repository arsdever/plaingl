#include <span>

#include "experimental/console/commands.hpp"

#include "experimental/console/commands/command.hpp"
#include "experimental/console/commands/new.hpp"
#include "experimental/console/commands/noop.hpp"

std::shared_ptr<command> command_center::parse_command()
{
    std::string command_string;
    command_string.reserve(1024);
    int ch = std::getchar();
    while (ch != '\n')
    {
        command_string.push_back(ch);
        ch = std::getchar();
    }

    return parse_command(command_string);
}

std::shared_ptr<command>
command_center::parse_command(std::string_view cmd_string)
{
    std::vector<std::string_view> args;
    args.reserve(128);
    std::size_t space_pos = cmd_string.find_first_of(' ');
    while (space_pos != std::string_view::npos)
    {
        args.push_back(cmd_string.substr(0, space_pos));
        cmd_string = cmd_string.substr(space_pos + 1);
        space_pos = cmd_string.find_first_of(' ');
    }

    args.push_back(cmd_string);

    return parse_command(args);
}

std::shared_ptr<command>
command_center::parse_command(const std::vector<std::string_view>& args)
{
    if (args[ 0 ] == "new")
    {
        return new_command::parse(args);
    }

    return std::make_shared<noop_command>();
}

void command_center::register_command(std::string command,
                                      std::function<void()> handler)
{
    _handler_map.emplace(std::move(command), handler);
}

template <>
int command_center::get<int>()
{
    std::string integer(32, '\0');
    size_t i = 0;
    if (std::isdigit(_last_char) || _last_char == '-')
    {
        integer[ i++ ] = _last_char;
    }

    while (std::isdigit(_last_char = std::getchar()))
    {
        integer[ i++ ] = _last_char;
    }

    return std::stoi(integer);
}

template <>
id command_center::get<id>()
{
    std::string id_str(16, '\0');
    size_t i = 0;
    while (std::isalnum(_last_char) && i < 16)
    {
        id_str[ i++ ] = _last_char;
        _last_char = std::getchar();
    }

    return { std::move(id_str) };
}

template <>
std::string command_center::get<std::string>()
{
    std::string result;
    while (isalnum(_last_char))
    {
        result.push_back(_last_char);
        _last_char = std::getchar();
    }

    return result;
}

std::unordered_map<std::string, std::function<void()>>
    command_center::_handler_map;
int command_center::_last_char { 0 };
