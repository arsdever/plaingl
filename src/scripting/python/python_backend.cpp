#include <entt/entt.hpp>
#include <pybind11/embed.h>

#include "scripting/python/python_backend.hpp"

#include "common/file.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"
#include "project/component.hpp"
#include "project/components/transform.hpp"

PYBIND11_EMBEDDED_MODULE(gamify, m)
{
    pybind11::class_<component>(m, "component")
        .def("transform", &component::get_transform);
    pybind11::class_<components::transform>(m, "transform")
        .def("rotate", &components::transform::rotate)
        .def("get_right", &components::transform::get_right);
}

namespace
{
logger log() { return get_logger("py"); }
} // namespace

namespace scripting
{
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
        auto result = module.attr("register")();

        for (auto& [ key, value ] : result.cast<pybind11::dict>())
        {
            auto component_name = key.cast<std::string>();
            log()->info("Found component type {} in the script",
                        component_name);

            // auto id = entt::hashed_string(component_name);
            // entt::meta<components::python_component>()
            //     .type(id)
            //     .base<components::python_component>()
            //     .ctor<&entt::registry::emplace_or_replace<components::c,
            //                                               game_object&>,
            //           entt::as_ref_t>()
            //     .func<entt::overload<components::c*(entt::entity)>(
            //         &entt::registry::try_get<components::c>)>(
            //         entt::hashed_string("try_get"))
            //     .func<&components::c ::serialize<json_serializer>>(
            //         entt::hashed_string("serialize"));
            // memory_manager::register_component_type(components::c::type_name);
            // return id;
        }
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
