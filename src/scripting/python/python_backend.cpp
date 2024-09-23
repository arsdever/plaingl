#include <pybind11/detail/common.h>
#include <pybind11/embed.h>

#include "scripting/python/python_backend.hpp"

#include "common/file.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"
#include "project/component_interface/component.hpp"
#include "project/component_interface/component_registry.hpp"
#include "project/game_object.hpp"
#include "scripting/python/python_component.hpp"
#include "scripting/script.hpp"

/* clang-format off */
#include "scripting/python/python_binding.hpp"
/* clang-format on */

namespace
{
logger log() { return get_logger("py"); }
} // namespace

extern pybind11::detail::embedded_module pybind11_module_gamify;

namespace scripting
{
backend::impl::impl()
    : _interpreter(std::make_unique<pybind11::scoped_interpreter>())
{
    component_registry::register_component("python_component",
                                           [](game_object& obj)
    { return std::make_shared<python_component>("python_component", obj); });
}

backend::impl::~impl() = default;

void backend::impl::load(std::string_view script_file_path)
{
    auto module_name = path_to_module_name(script_file_path);
    try
    {
        auto module = pybind11::module_::import(module_name.c_str());
        _modules.emplace(module_name, module);
        for (auto& it : module.attr("module_exports"))
        {
            auto cls = it.cast<pybind11::class_<python_component>>();
            component_registry::register_component(
                cls.attr("name").cast<std::string>(),
                [ cls ](game_object& obj) -> std::shared_ptr<component>
            {
                try
                {
                    auto pyobj = cls(obj.shared_from_this());
                    auto result =
                        pyobj.cast<std::shared_ptr<python_component>>();
                    result->_instance = pyobj;
                    return result;
                }
                catch (std::exception& e)
                {
                    log()->error("Failed to create component: {}", e.what());
                    return nullptr;
                }
            });
        }
    }
    catch (std::exception& e)
    {
        log()->error("Failed to load script: {}", e.what());
    }
}

void backend::impl::update(std::string_view script_file_path)
{
    auto module_name = path_to_module_name(script_file_path);
    auto it = _modules.find(module_name);
    bool was_loaded = it != _modules.end();
    if (!was_loaded)
    {
        load(script_file_path);
        return;
    }

    try
    {
        auto& module = it->second;
        module = pybind11::module_::import(module_name.c_str());
        for (auto& it : module.attr("module_exports"))
        {
            auto cls = it.cast<pybind11::class_<python_component>>();
            component_registry::register_component(
                cls.attr("name").cast<std::string>(),
                [ cls ](game_object& obj) -> std::shared_ptr<component>
            {
                try
                {
                    auto pyobj = cls(obj.shared_from_this());
                }
                catch (std::exception& e)
                {
                    log()->error("Failed to create component: {}", e.what());
                    return nullptr;
                }
            });
        }
    }
    catch (std::exception& e)
    {
        log()->error("Failed to load script: {}", e.what());
    }
}

std::string
backend::impl::path_to_module_name(std::string_view script_file_path)
{
    common::filesystem::path path { script_file_path };
    std::string relpath { path.full_path_without_filename().substr(
        common::filesystem::path::current_path().size() + 1) };
    std::replace(relpath.begin(), relpath.end(), '/', '.');
    relpath += ".";
    relpath += path.stem();
    return relpath;
}
} // namespace scripting
