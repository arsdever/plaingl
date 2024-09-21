#include <pybind11/embed.h>
#include <pybind11/operators.h>

#include "core/input_system.hpp"
#include "core/inputs/binding.hpp"
#include "project/components/transform.hpp"
#include "scripting/python/python_component.hpp"
#include "scripting/python/python_glm_binding.hpp"

using namespace scripting::python;

PYBIND11_EMBEDDED_MODULE(gamify, m)
{
    internal::expose_glm_type<glm::ivec2>::eval(m, "ivec2");
    internal::expose_glm_type<glm::vec2>::eval(m, "vec2");
    internal::expose_glm_type<glm::dvec2>::eval(m, "dvec2");
    internal::expose_glm_type<glm::uvec2>::eval(m, "uvec2");
    internal::expose_glm_type<glm::ivec3>::eval(m, "ivec3");
    internal::expose_glm_type<glm::vec3>::eval(m, "vec3");
    internal::expose_glm_type<glm::dvec3>::eval(m, "dvec3");
    internal::expose_glm_type<glm::uvec3>::eval(m, "uvec3");
    internal::expose_glm_type<glm::ivec4>::eval(m, "ivec4");
    internal::expose_glm_type<glm::vec4>::eval(m, "vec4");
    internal::expose_glm_type<glm::dvec4>::eval(m, "dvec4");
    internal::expose_glm_type<glm::uvec4>::eval(m, "uvec4");
    internal::expose_glm_type<glm::quat>::eval(m, "quat");
    internal::expose_glm_type<glm::dquat>::eval(m, "dquat");

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
        .def("set_rotation",
             pybind11::overload_cast<const glm::dquat&>(
                 &components::transform::set_rotation))
        .def("set_rotation",
             pybind11::overload_cast<const glm::dvec3&>(
                 &components::transform::set_rotation))
        .def("set_position", &components::transform::set_position)
        .def("move", &components::transform::move)
        .def("rotate", &components::transform::rotate)
        .def("right", &components::transform::get_right)
        .def("forward", &components::transform::get_forward)
        .def("up", &components::transform::get_up);

    auto is =
        pybind11::class_<core::input_system>(m, "input_system")
            .def("mouse_position", &core::input_system::get_mouse_position)
            .def("mouse_delta", &core::input_system::get_mouse_delta)
            .def("mouse_button", &core::input_system::get_mouse_button)
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
}
