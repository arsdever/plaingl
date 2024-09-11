#include "scripting/script.hpp"

namespace scripting
{
script::script(std::function<void(std::string_view)> message_handler)
    : _message_handler(std::move(message_handler))
{
}

void script::delegate_message(std::string_view message)
{
    _message_handler(message);
}
} // namespace scripting
