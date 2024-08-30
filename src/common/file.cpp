
#include "common/file.hpp"

#include "common/logging.hpp"
#include "file_watcher.hpp"

#if defined(WIN32)
#    include "common/impl/file_win.hpp"
#elif defined(__linux__)
#    include "common/impl/file_linux.hpp"
#endif

namespace common
{
file::file(std::string path)
    : _impl(std::make_unique<impl>(std::move(path)))
{
    try
    {
        _watcher =
            file_watcher(_impl->path,
                         [ this ](std::string_view path, file_change_type type)
                         { changed(type); });
    }
    catch (...)
    {
        _watcher = file_watcher();
    }
}

file::file(file&& o)
{
    _impl = std::move(o._impl);
    o._impl = std::make_unique<impl>("");
    _watcher = o._watcher;
}

file& file::operator=(file&& o) = default;

file::~file() { close(); }

bool file::is_open() const { return _impl->is_open(); }

void file::open(open_mode mode)
{
    _impl->open(mode);

    if (is_open() && !_watcher.is_valid())
    {
        _watcher =
            file_watcher(_impl->path,
                         [ this ](std::string_view path, file_change_type type)
                         { changed(type); });
        changed(file_change_type::created);
    }
}

void file::seek(size_t pos) { _impl->seek(pos); }

void file::close() { _impl->close(); }

void file::remove() { _impl->remove(); }

size_t file::read(char* buffer, size_t size)
{
    if (!is_open())
        open(file::open_mode::read);

    if (size == 0)
        return 0;

    return read_data(buffer, size);
}

size_t file::write(const char* buffer, size_t size)
{
    if (!is_open())
    {
        log()->warn("Failed to write to file {}: file is not open",
                    _impl->path);
        return 0;
    }

    if (size == 0)
        return 0;

    return _impl->write(buffer, size);
}

size_t file::get_size() const { return _impl->get_content_size(); }

std::string_view file::get_filepath() const { return _impl->path; }

size_t file::read_data(char* buffer, size_t length)
{
    return _impl->read(buffer, length);
}

file file::create(std::string_view path, std::string_view contents)
{
    file f { std::string(path) };

    if (f.exists(path))
        return f;

    f._impl = std::make_unique<impl>(impl::create(path, contents));
    return f;
}

void file::remove(std::string_view path) { impl::remove(path); }

bool file::exists() const { return impl::exists(_impl->path); }

bool file::exists(std::string_view path) { return impl::exists(path); }
} // namespace common
