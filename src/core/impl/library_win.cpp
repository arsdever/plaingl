#include <Windows.h>

#include "core/library.hpp"

namespace core
{
struct library::impl
{
    std::string path;
    HMODULE module;
};

library::library(std::string path)
{
    _impl = std::make_unique<impl>();
    _impl->path = std::move(path);
}

library::library(library&& o)
{
    _impl = std::make_unique<impl>();
    _impl->path = o._impl->path;
    _impl->module = o._impl->module;
    o._impl->module = 0;
}

library& library::operator=(library&& o)
{
    if (!_impl)
        _impl = std::make_unique<impl>();
    _impl->path = o._impl->path;
    _impl->module = o._impl->module;
    o._impl->module = 0;
    return *this;
}

library::~library() { release(); }

void library::lazy_load() { _impl->module = LoadLibrary(_impl->path.c_str()); }

void library::release()
{
    if (_impl->module)
        FreeLibrary(_impl->module);
    _impl->module = nullptr;
}

library::ftype library::resolve(std::string_view name)
{
    return GetProcAddress(_impl->module, name.data());
}
} // namespace core
