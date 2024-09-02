#include "common/directory.hpp"

#ifdef WIN32
#    include "common/impl/file_win.hpp"
#else
#    error No implementation is specified
#endif

namespace common
{

directory::directory(std::string path)
    : _impl(std::make_unique<impl>(std::string(path)))
{
}

directory::~directory() = default;

void directory::visit_files(std::function<void(std::string, bool)> cb)
{
    _impl->traverse(cb);
}
} // namespace common
