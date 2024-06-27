#include "graphics/graphics_channel.hpp"

#include "graphics/command.hpp"
#include "graphics/command_executors.hpp"

namespace graphics
{
void graphics_channel::post(command&& cmd)
{
    _command_buffer.push_back(std::move(cmd));
}

void graphics_channel::flush()
{
    for (auto& cmd : _command_buffer)
    {
        auto exec = graphics::command_executors::get(cmd._type);
        exec(cmd);
    }

    _command_buffer.clear();
}

graphics_channel& graphics_channel::instance()
{
    static graphics_channel instance;
    return instance;
}
} // namespace graphics
