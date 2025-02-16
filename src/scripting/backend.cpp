#include "scripting/backend.hpp"

#include "scripting/python/python_backend.hpp"

namespace scripting
{
backend::backend() { _impl = std::make_unique<impl>(); }

backend::~backend() = default;

void backend::initialize()
{
    _instance = std::unique_ptr<backend>(new backend());
}

void backend::shutdown() { _instance = nullptr; }

void backend::load_script(std::string_view path)
{
    if (_instance == nullptr)
    {
        get_logger("scripting")->info("Scripting backend is not initialized");
        return;
    }
    _instance->_impl->load(path);
}

std::unique_ptr<backend> backend::_instance { nullptr };
} // namespace scripting
