#include "core/command_dispatcher.hpp"

#include "core/command.hpp"

namespace core
{
void command_dispatcher::dispatch(std::shared_ptr<command_base> cmd)
{
    _command_queue.push(cmd);
}

void command_dispatcher::execute_all()
{
    while (!_command_queue.empty())
    {
        auto cmd = _command_queue.front();
        _command_queue.pop();
        cmd->execute();
    }
}
} // namespace core
