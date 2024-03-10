#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

struct id
{
    std::string _value;
};

class command;

class command_center
{
public:
    // ch - from previous input and to the next input
    template <typename T>
    static T get();

    static std::shared_ptr<command> parse_command();
    static std::shared_ptr<command> parse_command(std::string_view cmd_string);
    static std::shared_ptr<command>
    parse_command(const std::vector<std::string_view>& args);

    static void register_command(std::string command,
                                 std::function<void()> handler);

private:
    static std::unordered_map<std::string, std::function<void()>> _handler_map;
    static int _last_char;
};
