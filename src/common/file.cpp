
#include <FileWatch.hpp>

#include "common/file.hpp"

#include "common/logging.hpp"

namespace
{
static inline logger log() { return get_logger("fs"); }
file::event_type convert_to_local_event_type(filewatch::Event e)
{
    switch (e)
    {
    case filewatch::Event::added: return file::event_type::added;
    case filewatch::Event::modified: return file::event_type::modified;
    case filewatch::Event::removed: return file::event_type::removed;
    case filewatch::Event::renamed_new:
    case filewatch::Event::renamed_old: return file::event_type::renamed;
    default: return file::event_type::unknown;
    }
    return file::event_type::unknown;
};
} // namespace

struct file::private_data
{
    std::string _path {};
    mutable std::FILE* _descriptor { nullptr };
    std::unique_ptr<filewatch::FileWatch<std::string>> _watch;
    mutable state _state { state::invalid };
};

struct file::file_watch_hook::pdata
{
    filewatch::FileWatch<std::string> _watch;
};

file::file_watch_hook::file_watch_hook(pdata&& p)
    : _p(std::make_unique<pdata>(std::move(p)))
{
}

file::file_watch_hook&
file::file_watch_hook::operator=(file_watch_hook&&) = default;

file::file_watch_hook::file_watch_hook(file_watch_hook&&) = default;

file::file_watch_hook::~file_watch_hook() = default;

file::file(std::string path)
    : _pdata(std::make_unique<private_data>(
          std::move(path), nullptr, nullptr, state::invalid))
{
}

file::~file() { close(); }

void file::open(std::string_view mode) const
{
    bool mode_read = mode.find_first_of("r") != std::string::npos;
    if ((mode_read && _pdata->_state == state::open_read) ||
        (!mode_read && _pdata->_state == state::open_write))
    {
        return;
    }
    else if ((mode_read && _pdata->_state == state::open_write) ||
             (!mode_read && _pdata->_state == state::open_read))
    {
        log()->error("File is already open in a different mode");
        return;
    }

#ifdef WIN32
    auto error =
        fopen_s(&_pdata->_descriptor, _pdata->_path.data(), mode.data());
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
        log()->error("Failed to load the file at {}: {}",
                     _pdata->_path,
                     std::strerror(error));
        return;
#pragma clang diagnostic pop
    }
    _pdata->_state = mode_read ? state::open_read : state::open_write;
}

file::file_watch_hook file::watch(
    std::string_view path,
    std::function<void(std::string_view path, file::event_type event)> functor)
{
    return file::file_watch_hook({ filewatch::FileWatch<std::string>(
        std::string(path),
        [ functor ](const std::string& path, const filewatch::Event change_type)
    { functor(path, convert_to_local_event_type(change_type)); }) });
}

void file::close() const
{
    if (_pdata->_state != state::open_read &&
        _pdata->_state != state::open_write)
    {
        if (_pdata->_descriptor != nullptr)
        {
            log()->error(
                "File is not marked open, but the descriptor wasn't reset");
        }
        return;
    }

    fclose(_pdata->_descriptor);
    _pdata->_descriptor = nullptr;
    _pdata->_state = state::closed;
}

size_t file::size() const
{
    if (_pdata->_state != state::open_read)
    {
        log()->error("Can't get the size of a closed file");
        return -1;
    }

    int pos = std::ftell(_pdata->_descriptor);
    std::fseek(_pdata->_descriptor, 0, SEEK_END);
    int size = std::ftell(_pdata->_descriptor);
    std::fseek(_pdata->_descriptor, pos, SEEK_SET);
    return size;
}

std::string file::read_all() const
{
    if (_pdata->_state == state::open_read)
    {
        return read_all_open();
    }

    open("r");
    auto result = read_all_open();
    close();

    return result;
}

bool file::exists() const
{
    return std::filesystem::is_regular_file(_pdata->_path);
}

void file::watch()
{
    _pdata->_watch = std::make_unique<filewatch::FileWatch<std::string>>(
        _pdata->_path,
        [ this ](const std::string& path, filewatch::Event event_type)
    {
        if (event_type == filewatch::Event::renamed_new)
        {
            _pdata->_path = path;
        }
        changed_externally(convert_to_local_event_type(event_type));
    });
}

std::string file::read_all(std::string_view path)
{
    file f { std::string(path) };
    f.open("r");
    return f.read_all();
}

bool file::exists(std::string_view path)
{
    return std::filesystem::is_regular_file(path);
}

std::tuple<std::string, std::string, std::string>
file::parse_path(std::string_view path)
{
    std::filesystem::path p { path };
    return { p.root_directory().generic_string(),
             p.stem().generic_string(),
             p.extension().generic_string() };
}

std::string file::read_all_open() const
{
    size_t length = size();
    std::string result(length, '\0');
    std::fseek(_pdata->_descriptor, 0, SEEK_SET);
    std::fread(result.data(), 1, length, _pdata->_descriptor);
    return result;
}

event<void(std::string_view)> file::generic_file_change;
