#pragma once

#include "common/utils.hpp"

namespace scripting::python::internal
{
template <typename... T>
struct python_binding;

template <typename... T>
struct python_binding_traits;

template <typename... T>
struct type_binder
{
    static void eval(pybind11::module& m) { (type_binder<T>::eval(m), ...); }
};

template <typename... T>
struct type_binder<type_list<T...>>
{
    static void eval(pybind11::module& m) { (type_binder<T>::eval(m), ...); }
};

template <typename T>
struct type_binder<T>
{
    static void eval(pybind11::module& m)
    {
        if constexpr (requires { python_binding<T>::eval(m); })
        {
            python_binding<T>::eval(m);
        }
    }
};
} // namespace scripting::python::internal
