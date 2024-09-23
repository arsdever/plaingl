#include "scripting/backend.hpp"

namespace pybind11
{
class scoped_interpreter;
class module_;
} // namespace pybind11

namespace scripting
{
struct backend::impl
{
    impl();
    ~impl();

    void load(std::string_view script_file_path);
    void update(std::string_view script_file_path);

    std::unique_ptr<pybind11::scoped_interpreter> _interpreter;

private:
    std::string path_to_module_name(std::string_view path);

private:
    std::unordered_map<std::string, pybind11::module_> _modules;
};
} // namespace scripting
