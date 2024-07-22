#pragma once

#include <queue>

namespace core
{
class command_base;

class command_dispatcher
{
public:
    void dispatch(std::shared_ptr<command_base> cmd);
    void execute_all();

private:
    std::queue<std::shared_ptr<command_base>> _command_queue;
};
} // namespace core
