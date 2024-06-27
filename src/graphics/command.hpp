#pragma once

#include "graphics/commands/command_types.hpp"

namespace graphics
{
struct command
{
    command_type _type {};
    std::any _arguments {};
};
} // namespace graphics
