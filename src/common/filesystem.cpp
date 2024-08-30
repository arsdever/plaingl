#include "common/filesystem.hpp"

#include "common/impl/file_win.hpp"

namespace common::filesystem
{
path::path(std::string_view path)
    : _path(std::string(path))
{
    resolve();
}

path::path(const path& path)
{
    _path = path._path;
    resolve();
}

path& path::operator=(const path& path)
{
    _path = path._path;
    resolve();
    return *this;
}

path path::operator/(const path& p)
{
    return path(this->_path + "/") += p.relative_path();
}

path& path::operator/=(const path& p)
{
    _path = _path + "/";
    return operator+=(p.relative_path());
}

path path::operator/(std::string_view s)
{
    return path(this->_path + "/") += s;
}

path& path::operator/=(std::string_view s)
{
    _path = _path + "/";
    return operator+=(s);
}

path path::operator+(const path& p) { return path(*this) += p; }

path& path::operator+=(const path& p) { return *this += p.relative_path(); }

path path::operator+(std::string_view s) { return path(*this) += s; }

path& path::operator+=(std::string_view s)
{
    _path.insert(_path.end(), s.begin(), s.end());
    resolve();
    return *this;
}

void path::resolve()
{
    _full_path = filesystem_impl::convert_to_absolute_path(_path);
    _full_path_without_filename =
        std::string_view(_full_path).substr(0, _full_path.find_last_of('/'));
    _filename = std::string_view(_full_path)
                    .substr(_full_path_without_filename.size() + 1);
    if (auto pos = _filename.find_last_of('.'); pos != std::string::npos)
    {
        _stem = _filename.substr(0, pos);
        _extension = _filename.substr(pos);
    }
    _directory = _full_path_without_filename.substr(
        _full_path_without_filename.find_last_of('/') + 1);
}

path path::current_dir() { return path { filesystem_impl::current_dir() }; }
} // namespace common::filesystem
