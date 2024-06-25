#pragma once

#include "graphics/commands/command_type_traits.hpp"

namespace graphics
{
struct draw_elements_args
{
    int draw_type;
    int element_count;
    int element_type;
    int start_index;
};

namespace internal
{
template <>
struct command_type_traits<command_type::draw_elements>
{
    using type = draw_elements_args;
};
} // namespace internal
} // namespace graphics
