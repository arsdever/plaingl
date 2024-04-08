
#include "file.hpp"

#include "logging.hpp"

namespace
{
static inline logger log() { return get_logger("fs"); }
} // namespace

file::file() = default;

file::file(file&& o)
{
    _file_descriptor = o._file_descriptor;
    _state = o._state;
    o._file_descriptor = 0;
    o._state = state::invalid;
}

file& file::operator=(file&& o)
{
    _file_descriptor = o._file_descriptor;
    _state = o._state;
    o._file_descriptor = 0;
    o._state = state::invalid;
    return *this;
}

file::~file() { close(); }

void file::open(std::string_view path, std::string_view privileges)
{
    if (_state != state::closed)
    {
        log()->warn("Another file is already open. Closing");
        close();
    }

#ifdef WIN32
    auto error = fopen_s(&_file_descriptor, path.data(), privileges.data());
#else
    auto _file_descriptor = fopen(path.data(), privileges.data());
    int error;
    if (_file_descriptor)
    {
        error = errno;
    }
#endif

    if (error != 0)
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        log()->error(
            "Failed to load the file at {}: {}", path, std::strerror(error));
        return;
#pragma clang diagnostic pop
    }
    _state = state::open;
}

void file::close()
{
    fclose(_file_descriptor);
    _state = state::closed;
}

size_t file::size() const
{
    if (_state != state::open)
    {
        log()->error("Can't get the size of a closed file");
        return -1;
    }

    int pos = std::ftell(_file_descriptor);
    std::fseek(_file_descriptor, 0, SEEK_END);
    int size = std::ftell(_file_descriptor);
    std::fseek(_file_descriptor, pos, SEEK_SET);
    return size;
}

std::string file::read_all()
{
    if (_state != state::open)
    {
        log()->error("Can't read the contents of a closed file");
        return "";
    }
    size_t length = size();
    std::string result(length, '\0');
    std::fseek(_file_descriptor, 0, SEEK_SET);
    std::fread(result.data(), 1, length, _file_descriptor);

    return result;
}

std::string file::read_all(std::string_view path)
{
    file f;
    f.open(path, "r");
    return f.read_all();
}

bool file::exists(std::string_view path)
{
    return std::filesystem::is_regular_file(path);
}

std::tuple<std::string, std::string, std::string>
parse_path(std::string_view path)
{
    std::filesystem::path p { path };
    return { p.root_directory().generic_string(),
             p.stem().generic_string(),
             p.extension().generic_string() };
}
