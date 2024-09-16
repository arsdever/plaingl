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

    std::shared_ptr<script> load(common::file& f);

    std::unique_ptr<pybind11::scoped_interpreter> _interpreter;

private:
    std::vector<pybind11::module_> _modules;
};
} // namespace scripting
