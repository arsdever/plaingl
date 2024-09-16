#include <pybind11/pybind11.h>

#include "scripting/python/python_component.hpp"

#include "common/logging.hpp"

namespace
{
inline logger log() { return get_logger("python"); }
} // namespace

python_component::python_component(std::string_view name, game_object& obj)
    : component(name, obj)
{
}

void python_component::on_init() { }

void python_component::on_update() { }

void python_component::on_deinit() { }

namespace internal
{
void python_component_trampoline::on_init()
{
    try
    {
        PYBIND11_OVERRIDE(
            void,             /* Return type */
            python_component, /* Parent class */
            on_init /* Name of function in C++ (must match Python name) */
        );
    }
    catch (std::exception& e)
    {
        log()->error("{}", e.what());
    }
}

void python_component_trampoline::on_update()
{
    try
    {
        PYBIND11_OVERRIDE(
            void,             /* Return type */
            python_component, /* Parent class */
            on_update /* Name of function in C++ (must match Python name) */
        );
    }
    catch (std::exception& e)
    {
        log()->error("{}", e.what());
    }
}

void python_component_trampoline::on_deinit()
{
    try
    {
        PYBIND11_OVERRIDE(
            void,             /* Return type */
            python_component, /* Parent class */
            on_deinit /* Name of function in C++ (must match Python name) */
        );
    }
    catch (std::exception& e)
    {
        log()->error("{}", e.what());
    }
}
} // namespace internal
