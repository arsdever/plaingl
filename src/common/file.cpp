
#include "common/file.hpp"

#include "common/logging.hpp"

#ifdef WIN32
#    include "common/impl/file_win.hpp"
#endif

file::file(std::string path)
    : _impl(std::make_unique<impl>(std::move(path), nullptr))
{
}

file::file(file&& o)
{
    _impl = std::move(o._impl);
    o._impl = std::make_unique<impl>(_impl->path, nullptr);
}

file& file::operator=(file&& o) = default;

file::~file() { close(); }

bool file::is_open() const { return _impl->is_open(); }

void file::open(open_mode mode) { _impl->open(mode); }

size_t file::read(char* buffer, size_t size)
{
    if (!is_open())
        open(file::open_mode::read);

    return read_data(buffer, size);
}

void file::seek(size_t pos) { _impl->seek(pos); }

void file::close() { _impl->close(); }

size_t file::get_size() const { return _impl->get_content_size(); }

std::string_view file::get_filepath() const { return _impl->path; }

std::tuple<std::string, std::string, std::string>
file::parse_path(std::string_view path)
{
    std::filesystem::path p { path };
    return { p.root_directory().generic_string(),
             p.stem().generic_string(),
             p.extension().generic_string() };
}

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

bool file::exists() const { return impl::exists(_impl->path); }

bool file::exists(std::string_view path) { return impl::exists(path); }
