#include "experimental/console/commands/new.hpp"

#include "experimental/console/commands.hpp"
#include "experimental/console/commands/noop.hpp"
#include "experimental/window_manager.hpp"

void new_command::execute() { experimental::window_manager::create_window(); }

std::shared_ptr<command>
new_command::parse(const std::vector<std::string_view>& args)
{
    if (args[ 1 ] == "window")
    {
        return std::make_shared<new_command>();
    }

    return std::make_shared<noop_command>();
}
