#pragma once

#include "experimental/console/commands/command.hpp"

class noop_command : public command
{
public:
    void execute() override { }
};
