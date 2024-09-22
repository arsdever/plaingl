#pragma once

#include <glm/detail/qualifier.hpp>
#include <glm/gtc/quaternion.hpp>
#include <pybind11/detail/common.h>
#include <pybind11/embed.h>
#include <pybind11/operators.h>

#include "common/utils.hpp"

namespace scripting::python::internal
{
template <typename T>
struct expose_glm_type;

template <size_t N, typename T, glm::qualifier Q>
struct expose_glm_type<glm::vec<N, T, Q>>
{
    static void eval(pybind11::module& m, std::string_view python_name)
    {
        auto c =
            pybind11::class_<glm::vec<N, T, Q>>(m, python_name.data())
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
                .def(pybind11::self /= T());

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
struct expose_glm_type<glm::qua<T, Q>>
{
    static void eval(pybind11::module& m, std::string_view python_name)
    {
        auto c =
            pybind11::class_<glm::qua<T, Q>>(m, python_name.data())
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
                .def(pybind11::self /= T());
    }
};
} // namespace scripting::python::internal
