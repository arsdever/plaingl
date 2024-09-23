#include "scripting/script.hpp"

#include "common/filesystem.hpp"

namespace scripting
{
script::script(std::string_view path)
    : _file_path(path)
{
}

common::filesystem::path script::path() const { return _file_path; }
} // namespace scripting
