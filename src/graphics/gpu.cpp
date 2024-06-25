#include <glad/gl.h>

#include "graphics/gpu.hpp"

namespace graphics
{
std::string_view gpu::get_vendor()
{
    if (vendor.empty())
    {
        auto* ubytearray = glGetString(GL_VENDOR);
        vendor = std::string(reinterpret_cast<const char*>(ubytearray));
    }
    return vendor;
}

std::string_view gpu::get_device()
{
    if (device.empty())
    {
        auto* ubytearray = glGetString(GL_RENDERER);
        device = std::string(reinterpret_cast<const char*>(ubytearray));
    }
    return device;
}
} // namespace graphics
