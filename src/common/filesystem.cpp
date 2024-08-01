#include "common/filesystem.hpp"

#include "common/impl/file_win.hpp"

namespace common::filesystem
{
path::path(std::string_view path)
    : _path(std::string(path))
{
    _full_path = filesystem_impl::convert_to_absolute_path(path);

    _full_path_without_filename =
        std::string_view(_full_path).substr(0, _full_path.find_last_of('/'));
    _filename = std::string_view(_full_path)
                    .substr(_full_path_without_filename.size() + 1);
    if (auto pos = _filename.find_last_of('.'); pos != std::string::npos)
    {
        _stem = _filename.substr(0, pos);
        _extension = _filename.substr(pos + 1);
    }
    _directory = _full_path_without_filename.substr(
        _full_path_without_filename.find_last_of('/') + 1);
}

path path::current_dir() { return path { filesystem_impl::current_dir() }; }

std::tuple<std::string, std::string, std::string>
parse_path(std::string_view path)
{
    std::filesystem::path p { path };
    return { p.root_directory().generic_string(),
             p.stem().generic_string(),
             p.extension().generic_string() };
}
} // namespace common::filesystem
