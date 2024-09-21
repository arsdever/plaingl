#include <pybind11/embed.h>
#include <pybind11/operators.h>

#include "core/input_system.hpp"
#include "project/components/transform.hpp"
#include "scripting/python/python_component.hpp"

PYBIND11_EMBEDDED_MODULE(gamify, m)
{
    pybind11::class_<glm::ivec2>(m, "ivec2")
        .def(pybind11::init<int, int>())
        .def(pybind11::init<const glm::dvec2&>())
        .def(pybind11::init<const glm::vec2&>())
        .def_readwrite("x", &glm::ivec2::x)
        .def_readwrite("y", &glm::ivec2::y);

    pybind11::class_<glm::dvec2>(m, "vec2")
        .def(pybind11::init<double, double>())
        .def(pybind11::init<const glm::ivec2&>())
        .def(pybind11::init<const glm::vec2&>())
        .def_readwrite("x", &glm::dvec2::x)
        .def_readwrite("y", &glm::dvec2::y);

    pybind11::class_<glm::dvec3>(m, "vec3")
        .def(pybind11::init<double, double, double>())
        .def_readwrite("x", &glm::dvec3::x)
        .def_readwrite("y", &glm::dvec3::y)
        .def_readwrite("z", &glm::dvec3::z)
        .def(pybind11::self * double());

    pybind11::class_<glm::dquat>(m, "quat")
        .def(pybind11::init<double, double, double, double>())
        .def_readwrite("x", &glm::dquat::x)
        .def_readwrite("y", &glm::dquat::y)
        .def_readwrite("z", &glm::dquat::z)
        .def_readwrite("w", &glm::dquat::w);

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
        .def("rotate", &components::transform::rotate)
        .def("right", &components::transform::get_right)
        .def("forward", &components::transform::get_forward)
        .def("up", &components::transform::get_up);

    auto is =
        pybind11::class_<core::input_system>(m, "input_system")
            .def("mouse_position", &core::input_system::get_mouse_position)
            .def("mouse_delta", &core::input_system::get_mouse_delta)
            .def("mouse_button", &core::input_system::get_mouse_button);

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
}
