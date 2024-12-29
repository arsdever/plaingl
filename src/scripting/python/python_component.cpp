#include <pybind11/pybind11.h>

#include "scripting/python/python_component.hpp"

#include "common/logging.hpp"
#include "project/definitions.hpp"
#include "scripting/python/python_binding_interface.hpp"

namespace scripting::python
{
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

template <typename T, typename... U>
void try_assign(std::variant<U...>&& v, auto& obj, auto attr)
{
    if (std::holds_alternative<T>(v))
    {
        obj.attr(attr) = std::get<T>(std::move(v));
    }
}

template <typename... T, std::size_t... I>
void assign_variant(std::variant<T...>&& v,
                    auto& obj,
                    auto attr,
                    std::index_sequence<I...>)
{
    (try_assign<std::variant_alternative_t<I, std::variant<T...>>, T...>(
         std::move(v), obj, attr),
     ...);
}

void python_component::set_property_value(std::string_view name,
                                          trivial_types::variant_t value)
{
    try
    {
        assign_variant(std::move(value),
                       _instance,
                       name.data(),
                       std::make_index_sequence<
                           std::variant_size_v<trivial_types::variant_t>> {});
        return;
    }
    catch (std::exception& e)
    {
        log()->error("{}", e.what());
    }

    component::set_property_value(name, value);
}

namespace internal
{
void python_component_trampoline::on_init()
{
    try
    {
        PYBIND11_OVERRIDE(void, python_component, on_init);
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
        PYBIND11_OVERRIDE(void, python_component, on_update);
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
        PYBIND11_OVERRIDE(void, python_component, on_deinit);
    }
    catch (std::exception& e)
    {
        log()->error("{}", e.what());
    }
}

void python_component_trampoline::set_property_value(
    std::string_view name, trivial_types::variant_t value)
{
    try
    {
        PYBIND11_OVERRIDE(
            void, python_component, set_property_value, name, value);
    }
    catch (std::exception& e)
    {
        log()->error("{}", e.what());
    }
}
} // namespace internal
} // namespace scripting::python
