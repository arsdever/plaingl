#pragma once

namespace scripting
{
class script
{
public:
    script(std::function<void(std::string_view)> message_handler);

    void delegate_message(std::string_view message);

private:
    std::function<void(std::string_view)> _message_handler;
};
} // namespace scripting
