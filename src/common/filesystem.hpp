#pragma once

namespace common::filesystem
{
class path
{
public:
    path(std::string_view path);

    inline std::string_view filename() const { return _filename; }

    inline std::string_view stem() const { return _stem; }

    inline std::string_view extension() const { return _extension; }

    inline std::string_view directory() const { return _directory; }

    inline std::string_view full_path() const { return _full_path; }

    inline std::string_view full_path_without_filename() const
    {
        return _full_path_without_filename;
    }

    static path current_dir();

private:
    std::string _path;
    std::string _full_path;
    std::string_view _filename;
    std::string_view _stem;
    std::string_view _extension;
    std::string_view _directory;
    std::string_view _full_path_without_filename;
};

std::tuple<std::string, std::string, std::string>
parse_path(std::string_view path);
} // namespace common::filesystem
