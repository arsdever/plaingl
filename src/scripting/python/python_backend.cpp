#include <entt/entt.hpp>
#include <pybind11/detail/common.h>
#include <pybind11/embed.h>

#include "scripting/python/python_backend.hpp"

#include "common/file.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"
#include "project/component_interface/component.hpp"
#include "project/component_interface/component_registry.hpp"
#include "project/components/transform.hpp"
#include "project/game_object.hpp"

namespace
{
logger log() { return get_logger("py"); }
} // namespace

class python_component : public component
{
public:
    using base = component;

    python_component(game_object& obj)
        : component("python_component", obj)
    {
    }

    void on_init() override { }
    void on_update() override { }
    void on_deinit() override { }
};

class python_component_trampoline : public python_component
{
public:
    using base = python_component;
    using base::base;

    void on_init() override
    {
        PYBIND11_OVERRIDE(
            void,             /* Return type */
            python_component, /* Parent class */
            on_init /* Name of function in C++ (must match Python name) */
        );
    }
    void on_update() override
    {
        PYBIND11_OVERRIDE(
            void,             /* Return type */
            python_component, /* Parent class */
            on_update /* Name of function in C++ (must match Python name) */
        );
    }
    void on_deinit() override
    {
        PYBIND11_OVERRIDE(
            void,             /* Return type */
            python_component, /* Parent class */
            on_deinit /* Name of function in C++ (must match Python name) */
        );
    }
};

PYBIND11_EMBEDDED_MODULE(gamify, m)
{
    pybind11::class_<glm::dvec3>(m, "vec3");
    pybind11::class_<game_object, std::shared_ptr<game_object>>(m,
                                                                "game_object");
    pybind11::class_<component>(m, "component")
        .def("transform", &component::get_transform);
    pybind11::class_<python_component, python_component_trampoline>(
        m, "python_component")
        .def(pybind11::init<game_object&>())
        .def("on_init", &python_component::on_init)
        .def("on_update", &python_component::on_update)
        .def("on_deinit", &python_component::on_deinit)
        .def("transform", &python_component::get_transform);
    pybind11::class_<components::transform>(m, "transform")
        .def("rotate", &components::transform::rotate)
        .def("get_right", &components::transform::get_right);
}

namespace scripting
{
struct backend::impl::module_wrapper
{
    static python_component instantiate(std::string_view class_name,
                                        game_object& gobj)
    {
        auto& modules = backend::_instance->_impl->_modules;

        for (auto m : modules)
        {
            if (!m.attr("module_exports"))
            {
                continue;
            }

            for (auto c : m.attr("module_exports"))
            {
                auto type = c.cast<pybind11::class_<python_component>>();
                auto cn = type.attr("name").cast<std::string>();
                if (cn == class_name)
                {
                    return type(gobj).cast<python_component>();
                }
            }
        }

        throw std::runtime_error("class not found");
    }
};

backend::impl::impl()
    : _interpreter(std::make_unique<pybind11::scoped_interpreter>())
{
    initialize_project_module();
}

backend::impl::~impl() = default;

std::shared_ptr<script> backend::impl::load(common::file& f)
{
    common::filesystem::path path { f.get_filepath() };
    std::string relpath { path.full_path_without_filename().substr(
        common::filesystem::path::current_path().size() + 1) };
    std::replace(relpath.begin(), relpath.end(), '/', '.');
    relpath += ".";
    relpath += path.stem();
    try
    {
        auto module = pybind11::module_::import(relpath.c_str());
        _modules.emplace_back(module);
        for (auto& it : module.attr("module_exports"))
        {
            component_registry::register_component<python_component,
                                                   module_wrapper::instantiate>(
                it.attr("name").cast<std::string>());
        }
        // if (auto cclass =
        // class_obj.cast<pybind11::class_<python_component>>();
        //     cclass)
        // {
        //     log()->info("Found component type {} in the script", relpath);
        //     auto gobj = game_object::create();
        //     auto pobj = pybind11::cast(gobj);
        //     auto obj = cclass(gobj);
        //     log()->info("Python object: {}",
        //                 obj.attr("name").cast<std::string>());
        //     auto pc = obj.cast<std::shared_ptr<python_component>>();
        //     pc->on_update();
        //     log()->info("Python object: {}",
        //                 obj.attr("name").cast<std::string>());
        // }

        // for (auto& it : module.attr("module_exports"))
        // {
        //     if (auto clazz = it.cast<pybind11::class_<python_component>>();
        //         clazz)
        //     {
        //         log()->info("Registering type {}",
        //                     clazz.attr("name").cast<std::string>());
        //         project_manager::register_component_type(
        //             clazz.attr("name").cast<std::string>(),
        //             [ clazz ]() -> component& {});
        //     }
        // }

        //     for (auto& [ key, value ] : result.cast<pybind11::dict>())
        //     {
        //         auto component_name = key.cast<std::string>();
        //         log()->info("Found component type {} in the script",
        //                     component_name);

        //         component_registry::register_component<component>("component_name",
        //                                                           value.cast
        //                                                           <);

        //         // auto id = entt::hashed_string(component_name);
        //         // entt::meta<components::python_component>()
        //         //     .type(id)
        //         //     .base<components::python_component>()
        //         // .ctor<&entt::registry::emplace_or_replace<components::c,
        //         // game_object&>,
        //         //           entt::as_ref_t>()
        //         //     .func<entt::overload<components::c*(entt::entity)>(
        //         //         &entt::registry::try_get<components::c>)>(
        //         //         entt::hashed_string("try_get"))
        //         //     .func<&components::c ::serialize<json_serializer>>(
        //         //         entt::hashed_string("serialize"));
        //         //
        //         memory_manager::register_component_type(components::c::type_name);
        //         // return id;
        //     }
    }
    catch (std::exception& e)
    {
        log()->error("Failed to load script: {}", e.what());
        return {};
    }

    return {};
}

void backend::impl::initialize_project_module()
{
    // pybind11::class_<component>(*_interpreter, "component")
    //     .def("transform", &component::get_transform);
    // pybind11::class_<components::transform>(m, "transform")
    //     .def("rotate", &components::transform::rotate)
    //     .def("get_right", &components::transform::get_right);
}
} // namespace scripting
