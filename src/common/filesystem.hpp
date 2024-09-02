#pragma once

namespace common::filesystem
{
class path
{
public:
    path(std::string_view path);
    path(const path& path);
    path& operator=(const path& path);
    path(path&& path) = default;
    path& operator=(path&& path) = default;

    inline std::string_view relative_path() const { return _path; }

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
    static std::string current_path();

    path operator/(const path& p) const;
    path& operator/=(const path& p);
    path operator/(std::string_view s) const;
    path& operator/=(std::string_view s);
    path operator+(const path& p) const;
    path& operator+=(const path& p);
    path operator+(std::string_view s) const;
    path& operator+=(std::string_view s);

private:
    void resolve();

private:
    std::string _path;
    std::string _full_path;
    std::string_view _filename;
    std::string_view _stem;
    std::string_view _extension;
    std::string_view _directory;
    std::string_view _full_path_without_filename;
};
} // namespace common::filesystem
