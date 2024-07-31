#include "common/file_watcher.hpp"

#ifdef USE_FILEWATCH
#    include "impl/file_watcher_filewatch.hpp"
#endif

namespace common
{
file_watcher::file_watcher() = default;

file_watcher::file_watcher(
    std::string_view path,
    std::function<void(std::string_view, file_change_type)> functor)
    : _impl(std::make_unique<impl>(std::move(impl::create(path, functor))))
{
}

file_watcher& file_watcher::operator=(file_watcher&&) = default;

file_watcher::file_watcher(file_watcher&&) = default;

file_watcher::~file_watcher() = default;
} // namespace common
