#pragma once

namespace graphics
{
class gpu
{
public:
    static std::string_view get_vendor();
    static std::string_view get_device();

private:
    static std::string vendor;
    static std::string device;
};
} // namespace graphics
