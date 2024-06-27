#pragma once

#include "graphics/commands/commands.hpp"
#include "graphics/utils.hpp"

namespace graphics
{
class command_executors
{
public:
    using ftype = void (*)(command&);

    static ftype get(command_type cmd_code)
    {
        return _executors[ static_cast<int>(cmd_code) ];
    }

    template <command_type type>
    static internal::command_type_traits<type>::executor_type get()
    {
        return reinterpret_cast<
            internal::command_type_traits<type>::executor_type>(
            internal::command_type_traits<type>::executor);
    }

private:
    static constexpr std::array<void (*)(command&),
                                utils::enum_count<command_type>>
        _executors = utils::executors<command_type>::value;
};
} // namespace graphics
