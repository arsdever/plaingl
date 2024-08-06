#pragma once

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common/file.hpp"
#include "common/logging.hpp"

namespace common
{
namespace
{
inline logger log() { return get_logger("file"); }
} // namespace

struct filesystem_impl
{
    static std::string convert_to_absolute_path(std::string_view path)
    {
        return std::filesystem::absolute(path).string();
    }

    static std::string current_dir()
    {
        std::string result;
        result.resize(PATH_MAX);
        auto sz = ::getcwd(result.data(), PATH_MAX);
        result.shrink_to_fit();
        return result;
    }
};

struct file::impl
{
    std::string path {};
    int file { 0 };

    bool is_open() { return file != 0; }

    void open(file::open_mode mode)
    {
        int lnx_mode = 0;
        switch (mode)
        {
        case file::open_mode::read: lnx_mode = O_RDONLY; break;
        case file::open_mode::write: lnx_mode = O_WRONLY; break;
        case file::open_mode::read_write: lnx_mode = O_RDWR; break;
        case file::open_mode::append: lnx_mode = O_APPEND; break;
        default: break;
        }

        file = ::open(path.data(), lnx_mode);

        if (file == -1)
        {
            log()->error("Error opening file {}: {}", path, strerror(errno));
            file = 0;
        }
    }

    void close()
    {
        if (file == 0)
            return;

        ::close(file);
        file = 0;
    }

    void remove()
    {
        close();

        remove(path);
    }

    static void remove(std::string_view path)
    {
        if (unlink(path.data()) == -1)
        {
            log()->error("Error removing file {}: {}", path, strerror(errno));
        }
    }

    size_t get_content_size()
    {
        if (file == 0)
            return 0;

        auto pos = ::lseek(file, 0, SEEK_END);
        if (pos == -1)
        {
            log()->error("Error getting file size: {}", strerror(errno));
        }
        return pos;
    }

    size_t read(void* buffer, size_t size)
    {
        if (file == 0)
            return 0;

        ssize_t bytes_read = ::read(file, buffer, size);
        if (bytes_read == -1)
        {
            log()->error("Error reading file {}: {}", path, strerror(errno));
        }

        return static_cast<size_t>(bytes_read);
    }

    size_t write(const void* buffer, size_t size)
    {
        if (file == 0)
            return 0;

        ssize_t bytes_written = ::write(file, buffer, size);
        if (bytes_written == -1)
        {
            log()->error("Error writing to file {}: {}", path, strerror(errno));
        }

        return static_cast<size_t>(bytes_written);
    }

    void seek(size_t position)
    {
        if (file == 0)
            return;

        ::lseek(file, position, SEEK_SET);
    }

    static impl create(std::string_view path, std::string_view contents)
    {
        int file = ::open(path.data(), O_WRONLY | O_CREAT | O_TRUNC, 0666);

        if (file == -1)
        {
            log()->error("Error creating file {}: {}", path, strerror(errno));
            return {};
        }

        impl i { std::string(path), file };
        i.write(contents.data(), contents.size());

        return std::move(i);
    }

    static std::string current_dir() { return filesystem_impl::current_dir(); }

    static bool exists(std::string_view path)
    {
        // https://stackoverflow.com/a/12774387/10185183
        struct stat buffer;
        return (stat(path.data(), &buffer) == 0);
    }
};
} // namespace common
