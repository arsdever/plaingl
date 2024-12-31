#include <pybind11/detail/common.h>
#include <pybind11/embed.h>

#include "scripting/python/python_backend.hpp"

#include "common/file.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"
#include "common/main_thread_dispatcher.hpp"
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

std::shared_ptr<script> backend::impl::load(std::string_view script_file_path)
{
    auto result = std::make_shared<script>(script_file_path);

    auto module_name = path_to_module_name(script_file_path);
    try
    {
        _modules[ module_name ] =
            pybind11::module_::import(module_name.c_str());
        for (auto& it : _modules[ module_name ].attr("module_exports"))
        {
            auto cls = it.cast<pybind11::class_<python_component>>();
            register_component(cls);
        }
    }
    catch (std::exception& e)
    {
        log()->error("Failed to load script: {}", e.what());
    }

    return result;
}

void backend::impl::update(std::string_view script_file_path)
{
    std::string module_name = path_to_module_name(script_file_path);
    common::main_thread_dispatcher::dispatch([ this, module_name ]
    {
        try
        {
            pybind11::module module;
            if (auto it = _modules.find(module_name); it != _modules.end())
            {
                if (it->second)
                {
                    log()->info("Reloading module {}", module_name);
                    it->second.reload();
                    module = it->second;
                }
            }
            else
            {
                log()->info("Module {} not loaded", module_name);
                module = pybind11::module_::import(module_name.c_str());
            }

            for (auto& it : module.attr("module_exports"))
            {
                auto cls = it.cast<pybind11::class_<python_component>>();
                auto cls_name = cls.attr("name").cast<std::string>();
                std::vector<std::reference_wrapper<game_object>>
                    _owning_game_objects {};
                auto cmp = _bindings.find(cls_name);
                if (cmp != _bindings.end())
                {
                    auto bkt = _bindings.bucket(cmp->first);
                    for (auto it = _bindings.begin(bkt);
                         it != _bindings.end(bkt);
                         ++it)
                    {
                        if (it->first == cmp->first)
                        {
                            auto& go = cmp->second->get_game_object();
                            _owning_game_objects.emplace_back(go);
                            go.remove(it->second);
                        }
                    }
                }

                component_registry::unregister_component(
                    cls.attr("name").cast<std::string>());
                register_component(cls);

                for (auto& go : _owning_game_objects)
                {
                    go.get().add(cls_name);
                }
            }
        }
        catch (std::exception& e)
        {
            log()->error("Failed to load script: {}", e.what());
        }
    });
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

template <typename T, typename... Extra>
void backend::impl::register_component(pybind11::class_<T, Extra...> cls)
{
    component_registry::register_component(
        cls.attr("name").template cast<std::string>(),
        [ this, cls ](game_object& obj) -> std::shared_ptr<component>
    {
        try
        {
            auto pyobj = cls(obj.shared_from_this());
            auto result =
                pyobj.template cast<std::shared_ptr<python_component>>();
            result->_instance = pyobj;
            _bindings.emplace(cls.attr("name").template cast<std::string>(),
                              result);
            return result;
        }
        catch (std::exception& e)
        {
            log()->error("Failed to create component: {}", e.what());
            return nullptr;
        }
    });
}
} // namespace scripting
