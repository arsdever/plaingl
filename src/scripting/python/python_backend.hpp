#include "scripting/backend.hpp"
#include "scripting/python/python_component.hpp"

namespace pybind11
{
class scoped_interpreter;
class module_;
template <typename T, typename... Extra>
class class_;
} // namespace pybind11

namespace scripting
{
namespace python
{
class python_component;
}

struct backend::impl
{
    impl();
    ~impl();

    std::shared_ptr<script> load(std::string_view script_file_path);
    void update(std::string_view script_file_path);

    std::unique_ptr<pybind11::scoped_interpreter> _interpreter;

private:
    std::string path_to_module_name(std::string_view path);

    template <typename T, typename... Extra>
    void register_component(pybind11::class_<T, Extra...> cls);

private:
    std::unordered_map<std::string, pybind11::module> _modules;
    std::unordered_multimap<std::string,
                            std::shared_ptr<python::python_component>>
        _bindings;
};
} // namespace scripting
