#pragma once

#include "common/filesystem.hpp"

namespace scripting
{
class script
{
public:
    script(std::string_view path);

    common::filesystem::path path() const;

private:
    common::filesystem::path _file_path;
};
} // namespace scripting
