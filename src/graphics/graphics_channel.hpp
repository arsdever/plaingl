#pragma once

#include "graphics/command.hpp"

namespace graphics
{
class graphics_channel
{
public:
    template <typename... ARGS>
    void post(ARGS&&... args)
    {
        post(command { std::forward<ARGS>(args)... });
    }

    void post(command&& cmd);

	void flush();

    static graphics_channel& instance();

private:
    // TODO: std::vector is not the best choice
    std::vector<command> _command_buffer;
};
} // namespace graphics
