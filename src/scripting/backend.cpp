#include "scripting/backend.hpp"

#include "common/file.hpp"
#include "scripting/python/python_backend.hpp"
#include "scripting/script.hpp"

namespace scripting
{
backend::backend() { _impl = std::make_unique<impl>(); }

backend::~backend() = default;

void backend::initialize()
{
    _instance = std::unique_ptr<backend>(new backend());
}

void backend::shutdown() { _instance = nullptr; }

std::shared_ptr<script> backend::load_script(common::file& f)
{
    return _instance->_impl->load(f.get_filepath());
}

void backend::update_script(std::shared_ptr<script> s)
{
    return _instance->_impl->update(s->path().full_path());
}

std::unique_ptr<backend> backend::_instance { nullptr };
} // namespace scripting
