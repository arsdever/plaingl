#include <pybind11/chrono.h>
#include <pybind11/complex.h>
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "core/game_clock.hpp"
#include "core/input_system.hpp"
#include "core/inputs/binding.hpp"
#include "graphics/material.hpp"
#include "graphics/mesh.hpp"
#include "graphics/vertex.hpp"
#include "project/components/mesh_filter.hpp"
#include "project/components/mesh_renderer.hpp"
#include "project/components/transform.hpp"
#include "project/definitions.hpp"
#include "scripting/python/python_binding_interface.hpp"
#include "scripting/python/python_component.hpp"
#include "scripting/python/python_glm_binding.hpp"

using namespace scripting::python;

PYBIND11_EMBEDDED_MODULE(gamify, m)
{
    internal::type_binder<trivial_types>::eval(m);

    pybind11::class_<game_object, std::shared_ptr<game_object>>(m,
                                                                "game_object");

    pybind11::class_<component, std::shared_ptr<component>>(m, "component");

    pybind11::class_<python_component,
                     internal::python_component_trampoline,
                     std::shared_ptr<python_component>>(m, "python_component")
        .def(pybind11::init<std::string_view, game_object&>())
        .def("on_init", &python_component::on_init)
        .def("on_update", &python_component::on_update)
        .def("on_deinit", &python_component::on_deinit)
        .def("transform", &python_component::get_transform)
        .def("get",
             static_cast<std::shared_ptr<component> (python_component::*)(
                 std::string_view)>(&python_component::get));

    pybind11::class_<components::transform,
                     std::shared_ptr<components::transform>>(m, "transform")
        .def("set_position", &components::transform::set_position)
        .def("set_rotation",
             pybind11::overload_cast<const glm::dquat&>(
                 &components::transform::set_rotation))
        .def("set_rotation",
             pybind11::overload_cast<const glm::dvec3&>(
                 &components::transform::set_rotation))
        .def("set_scale", &components::transform::set_scale)
        .def("position",
             &components::transform::get_position<
                 components::transform::relation_flag::world>)
        .def("local_position",
             &components::transform::get_position<
                 components::transform::relation_flag::local>)
        .def("rotation",
             &components::transform::get_rotation<
                 components::transform::relation_flag::world>)
        .def("local_rotation",
             &components::transform::get_rotation<
                 components::transform::relation_flag::local>)
        .def("scale",
             &components::transform::get_scale<
                 components::transform::relation_flag::world>)
        .def("local_scale",
             &components::transform::get_scale<
                 components::transform::relation_flag::local>)
        .def("move", &components::transform::move)
        .def("rotate", &components::transform::rotate)
        .def("right", &components::transform::get_right)
        .def("forward", &components::transform::get_forward)
        .def("up", &components::transform::get_up);

    pybind11::class_<components::mesh_filter,
                     std::shared_ptr<components::mesh_filter>>(m, "mesh_filter")
        .def("set_mesh", &components::mesh_filter::set_mesh)
        .def("mesh", &components::mesh_filter::get_mesh);

    pybind11::class_<components::mesh_renderer,
                     std::shared_ptr<components::mesh_renderer>>(
        m, "mesh_renderer")
        .def("set_material", &components::mesh_renderer::set_material)
        .def("material", &components::mesh_renderer::get_material);

    auto is =
        pybind11::class_<core::input_system>(m, "input_system")
            .def("mouse_position", &core::input_system::get_mouse_position)
            .def("mouse_delta", &core::input_system::get_mouse_delta)
            .def("mouse_button", &core::input_system::get_mouse_button)
            .def("bind",
                 pybind11::overload_cast<std::string_view, std::string_view>(
                     &core::input_system::bind_input))
            .def("get", &core::input_system::get_input);

    pybind11::enum_<core::input_system::mouse_button>(is, "mouse_button_code")
        .value("MouseButton0", core::input_system::mouse_button::MouseButton0)
        .value("MouseButton1", core::input_system::mouse_button::MouseButton1)
        .value("MouseButton2", core::input_system::mouse_button::MouseButton2)
        .value("MouseButton3", core::input_system::mouse_button::MouseButton3)
        .value("MouseButton4", core::input_system::mouse_button::MouseButton4)
        .value("MouseButton5", core::input_system::mouse_button::MouseButton5)
        .value("MouseButton6", core::input_system::mouse_button::MouseButton6)
        .value("MouseButton7", core::input_system::mouse_button::MouseButton7)
        .value("MouseButton8", core::input_system::mouse_button::MouseButton8)
        .value("MouseButton9", core::input_system::mouse_button::MouseButton9)
        .value("MouseButtonLeft",
               core::input_system::mouse_button::MouseButtonLeft)
        .value("MouseButtonRight",
               core::input_system::mouse_button::MouseButtonRight)
        .value("MouseButtonMiddle",
               core::input_system::mouse_button::MouseButtonMiddle);

    pybind11::enum_<core::input_system::button_state>(is, "button_state")
        .value("Unspecified", core::input_system::button_state::Unspecified)
        .value("Press", core::input_system::button_state::Press)
        .value("Hold", core::input_system::button_state::Hold)
        .value("Release", core::input_system::button_state::Release);

    pybind11::class_<core::binding, std::shared_ptr<core::binding>>(
        m, "input_binding")
        .def("get_int", &core::binding::get<int>)
        .def("get_uint", &core::binding::get<unsigned>)
        .def("get_float", &core::binding::get<float>)
        .def("get_vec2", &core::binding::get<glm::dvec2>)
        .def("get_vec3", &core::binding::get<glm::dvec3>)
        .def("get_vec4", &core::binding::get<glm::dvec4>);

    pybind11::class_<vertex3d>(m, "vertex")
        .def(pybind11::init<glm::vec3>())
        .def(pybind11::init<float, float, float>())
        .def_property(
            "position", &vertex3d::get_position, &vertex3d::set_position)
        .def_property("normal", &vertex3d::get_normal, &vertex3d::set_normal)
        .def_property("uv", &vertex3d::get_uv, &vertex3d::set_uv)
        .def_property("color", &vertex3d::get_color, &vertex3d::set_color)
        .def_property(
            "bitangent", &vertex3d::get_bitangent, &vertex3d::set_bitangent)
        .def_property(
            "tangent", &vertex3d::get_tangent, &vertex3d::set_tangent);

    pybind11::class_<graphics::mesh, std::shared_ptr<graphics::mesh>>(m, "mesh")
        .def(pybind11::init<>())
        .def("set_vertices", &graphics::mesh::set_vertices)
        .def("set_indices", &graphics::mesh::set_indices)
        .def("recalculate_normals", &graphics::mesh::recalculate_normals)
        .def("init", &graphics::mesh::init);

    pybind11::class_<graphics::material, std::shared_ptr<graphics::material>>(
        m, "material")
        .def(pybind11::init<>())
        .def("set_property",
             static_cast<void (graphics::material::*)(std::string_view,
                                                      glm::vec2)>(
                 &graphics::material::set_property_value<glm::vec2>))
        .def("set_property",
             static_cast<void (graphics::material::*)(std::string_view,
                                                      glm::vec3)>(
                 &graphics::material::set_property_value<glm::vec3>))
        .def("set_property",
             static_cast<void (graphics::material::*)(std::string_view,
                                                      glm::vec4)>(
                 &graphics::material::set_property_value<glm::vec4>));

    pybind11::class_<game_clock>(m, "clock")
        .def_static("delta", &game_clock::delta)
        .def_static("absolute", &game_clock::absolute)
        .def_static("physics_delta", &game_clock::physics_delta);
}
