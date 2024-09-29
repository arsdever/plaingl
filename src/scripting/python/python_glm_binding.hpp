#pragma once

#include <glm/detail/qualifier.hpp>
#include <glm/gtc/quaternion.hpp>
#include <pybind11/detail/common.h>
#include <pybind11/embed.h>
#include <pybind11/operators.h>

#include "scripting/python/python_binding_interface.hpp"

namespace scripting::python::internal
{
template <size_t N, typename T, glm::qualifier Q>
struct python_binding<glm::vec<N, T, Q>>
{
    static void eval(pybind11::module& m)
    {
        auto c =
            pybind11::class_<glm::vec<N, T, Q>>(
                m, python_binding_traits<glm::vec<N, T, Q>>::name)
                .def(pybind11::init())
                .def(pybind11::init<T>())
                .def(pybind11::init<const glm::vec<N, char, Q>&>())
                .def(pybind11::init<const glm::vec<N, short, Q>&>())
                .def(pybind11::init<const glm::vec<N, int, Q>&>())
                .def(pybind11::init<const glm::vec<N, long, Q>&>())
                .def(pybind11::init<const glm::vec<N, long long, Q>&>())
                .def(pybind11::init<const glm::vec<N, unsigned char, Q>&>())
                .def(pybind11::init<const glm::vec<N, unsigned short, Q>&>())
                .def(pybind11::init<const glm::vec<N, unsigned int, Q>&>())
                .def(pybind11::init<const glm::vec<N, unsigned long, Q>&>())
                .def(
                    pybind11::init<const glm::vec<N, unsigned long long, Q>&>())
                .def(pybind11::init<const glm::vec<N, float, Q>&>())
                .def(pybind11::init<const glm::vec<N, double, Q>&>())
                .def_readwrite("x", &glm::vec<N, T, Q>::x)
                .def(pybind11::self - glm::vec<N, T, Q>())
                .def(pybind11::self - T())
                .def(pybind11::self + glm::vec<N, T, Q>())
                .def(pybind11::self + T())
                .def(pybind11::self * glm::vec<N, T, Q>())
                .def(pybind11::self * T())
                .def(pybind11::self / glm::vec<N, T, Q>())
                .def(pybind11::self / T())
                .def(pybind11::self += glm::vec<N, T, Q>())
                .def(pybind11::self += T())
                .def(pybind11::self -= glm::vec<N, T, Q>())
                .def(pybind11::self -= T())
                .def(pybind11::self *= glm::vec<N, T, Q>())
                .def(pybind11::self *= T())
                .def(pybind11::self /= glm::vec<N, T, Q>())
                .def(pybind11::self /= T())
                .def("__repr__",
                     [](const glm::vec<N, T, Q>& v)
        { return glm::to_string(v); });

        if constexpr (N > 1)
        {
            c.def_readwrite("y", &glm::vec<N, T, Q>::y);
        }
        if constexpr (N > 2)
        {
            c.def_readwrite("z", &glm::vec<N, T, Q>::z);
        }
        if constexpr (N > 3)
        {
            c.def_readwrite("w", &glm::vec<N, T, Q>::w);
        }

        if constexpr (N == 2)
        {
            c.def(pybind11::init<T, T>());
        }
        else if constexpr (N == 3)
        {
            c.def(pybind11::init<T, T, T>());
            c.def(pybind11::init<T, const glm::vec<2, T, Q>&>());
            c.def(pybind11::init<const glm::vec<2, T, Q>&, T>());
        }
        else if constexpr (N == 4)
        {
            c.def(pybind11::init<T, T, T, T>());
            c.def(pybind11::init<const glm::vec<2, T, Q>&, T, T>());
            c.def(pybind11::init<T, const glm::vec<2, T, Q>&, T>());
            c.def(pybind11::init<T, T, const glm::vec<2, T, Q>&>());
            c.def(pybind11::init<const glm::vec<2, T, Q>&,
                                 const glm::vec<2, T, Q>&>());
            c.def(pybind11::init<const glm::vec<3, T, Q>&, T>());
            c.def(pybind11::init<T, const glm::vec<3, T, Q>&>());
        }
    }
};

template <typename T, glm::qualifier Q>
struct python_binding<glm::qua<T, Q>>
{
    static void eval(pybind11::module& m)
    {
        auto c =
            pybind11::class_<glm::qua<T, Q>>(
                m, python_binding_traits<glm::qua<T, Q>>::name)
                .def(pybind11::init())
                .def(pybind11::init<const glm::qua<char, Q>&>())
                .def(pybind11::init<const glm::qua<short, Q>&>())
                .def(pybind11::init<const glm::qua<int, Q>&>())
                .def(pybind11::init<const glm::qua<long, Q>&>())
                .def(pybind11::init<const glm::qua<long long, Q>&>())
                .def(pybind11::init<const glm::qua<unsigned char, Q>&>())
                .def(pybind11::init<const glm::qua<unsigned short, Q>&>())
                .def(pybind11::init<const glm::qua<unsigned int, Q>&>())
                .def(pybind11::init<const glm::qua<unsigned long, Q>&>())
                .def(pybind11::init<const glm::qua<unsigned long long, Q>&>())
                .def(pybind11::init<const glm::qua<float, Q>&>())
                .def(pybind11::init<const glm::qua<double, Q>&>())
                .def(pybind11::init<T, T, T, T>())
                .def(pybind11::init<T, const glm::vec<3, T, Q>&>())
                .def(pybind11::init<const glm::vec<3, T, Q>&,
                                    const glm::vec<3, T, Q>&>())
                .def(pybind11::init<const glm::vec<3, T, Q>&>())
                .def_readwrite("x", &glm::qua<T, Q>::x)
                .def_readwrite("y", &glm::qua<T, Q>::y)
                .def_readwrite("z", &glm::qua<T, Q>::z)
                .def_readwrite("w", &glm::qua<T, Q>::w)
                .def("euler_angles",
                     [](const glm::qua<T, Q>& q)
        { return glm::eulerAngles(q); })
                .def(pybind11::self + glm::qua<T, Q>())
                .def(pybind11::self - glm::qua<T, Q>())
                .def(pybind11::self * glm::qua<T, Q>())
                .def(pybind11::self * glm::vec<3, T, Q>())
                .def(pybind11::self * glm::vec<4, T, Q>())
                .def(pybind11::self * T())
                .def(pybind11::self / T())
                .def(pybind11::self += glm::qua<T, Q>())
                .def(pybind11::self -= glm::qua<T, Q>())
                .def(pybind11::self *= glm::qua<T, Q>())
                .def(pybind11::self *= T())
                .def(pybind11::self /= T())
                .def("__repr__",
                     [](const glm::qua<T, Q>& v) { return glm::to_string(v); });
        ;
    }
};

template <>
struct python_binding_traits<glm::ivec2>
{
    static constexpr auto name = "ivec2";
};

template <>
struct python_binding_traits<glm::vec2>
{
    static constexpr auto name = "vec2";
};

template <>
struct python_binding_traits<glm::dvec2>
{
    static constexpr auto name = "dvec2";
};

template <>
struct python_binding_traits<glm::uvec2>
{
    static constexpr auto name = "uvec2";
};

template <>
struct python_binding_traits<glm::ivec3>
{
    static constexpr auto name = "ivec3";
};

template <>
struct python_binding_traits<glm::vec3>
{
    static constexpr auto name = "vec3";
};

template <>
struct python_binding_traits<glm::dvec3>
{
    static constexpr auto name = "dvec3";
};

template <>
struct python_binding_traits<glm::uvec3>
{
    static constexpr auto name = "uvec3";
};

template <>
struct python_binding_traits<glm::ivec4>
{
    static constexpr auto name = "ivec4";
};

template <>
struct python_binding_traits<glm::vec4>
{
    static constexpr auto name = "vec4";
};

template <>
struct python_binding_traits<glm::dvec4>
{
    static constexpr auto name = "dvec4";
};

template <>
struct python_binding_traits<glm::uvec4>
{
    static constexpr auto name = "uvec4";
};

template <>
struct python_binding_traits<glm::quat>
{
    static constexpr auto name = "quat";
};

template <>
struct python_binding_traits<glm::dquat>
{
    static constexpr auto name = "dquat";
};

} // namespace scripting::python::internal
