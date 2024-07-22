#pragma once

#include <queue>

namespace core
{
class command;

class command_dispatcher
{
public:
    void dispatch(std::shared_ptr<command> cmd);
    void execute_all();

private:
    std::queue<std::shared_ptr<command>> _command_queue;
};
} // namespace core
