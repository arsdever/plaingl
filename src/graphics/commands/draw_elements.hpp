#pragma once

#include "graphics/command.hpp"
#include "graphics/commands/command_type_traits.hpp"

namespace graphics
{
struct draw_elements_args
{
    int draw_type;
    int element_count;
    int element_type;
    const void* start_index;
};

namespace internal
{
template <>
struct command_type_traits<command_type::draw_elements>
{
    using type = draw_elements_args;
    static constexpr void (*executor)(command&) = [](command& cmd)
    {
        auto args = std::any_cast<draw_elements_args>(cmd._arguments);
        glDrawElements(args.draw_type,
                       args.element_count,
                       args.element_type,
                       args.start_index);
    };
};
} // namespace internal
} // namespace graphics
