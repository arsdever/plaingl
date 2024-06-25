#pragma once

namespace graphics
{
enum class command_type
{
    draw_elements
};

struct draw_elements_args
{
    int draw_type;
    int element_count;
    int element_type;
    int start_index;
};

template <command_type cmd>
struct command_type_traits;

template <command_type... types>
using command_arg = std::variant<typename command_type_traits<types>::type...>;

template <>
struct command_type_traits<command_type::draw_elements>
{
    using type = draw_elements_args;
};

struct command
{
    command_type _type {};
    command_arg<command_type::draw_elements> _draw_elements {};
};

extern std::vector<command> command_buffer;
} // namespace graphics
