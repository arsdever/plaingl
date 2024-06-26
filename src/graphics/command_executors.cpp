#include <unordered_map>

#include "graphics/command_executors.hpp"

#include "graphics/commands/command_type_traits.hpp"

namespace graphics
{
command_executor& command_executors::get(command_type type)
{
    return _executors.at(type);
}

constexpr std::array<command_executor, utils::enum_count<command_type>>
    command_executors::_executors = build_executors_lut();
} // namespace graphics
