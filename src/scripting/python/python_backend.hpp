#include "scripting/backend.hpp"

namespace pybind11
{
class scoped_interpreter;
}

namespace scripting
{
struct backend::impl
{
    impl();
    ~impl();

    std::shared_ptr<script> load(common::file& f);

    std::unique_ptr<pybind11::scoped_interpreter> _interpreter;

private:
    void initialize_project_module();
};
} // namespace scripting
