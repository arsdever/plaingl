#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "experimental/console/commands/command.hpp"

class new_command : public command
{
public:
    void execute() override;

    static std::shared_ptr<command>
    parse(const std::vector<std::string_view>& args);
};
