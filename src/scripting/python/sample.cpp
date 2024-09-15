#include <pybind11/detail/common.h>
#include <pybind11/embed.h>

#include "common/logging.hpp"
#include "project/component_interface/component.hpp"
#include "project/component_interface/component_registry.hpp"
#include "project/components/transform.hpp"
#include "project/game_object.hpp"

class python_component : public component
{
public:
    using base = component;

    python_component(game_object& obj)
        : component("python_component", obj)
    {
    }

    void on_init() override { }
    void on_update() override { }
    void on_deinit() override { }

private:
    friend int main();
    pybind11::object _instance;
};

class python_component_trampoline : public python_component
{
public:
    using base = python_component;
    using base::base;

    void on_init() override
    {
        PYBIND11_OVERRIDE(
            void,             /* Return type */
            python_component, /* Parent class */
            on_init /* Name of function in C++ (must match Python name) */
        );
    }
    void on_update() override
    {
        PYBIND11_OVERRIDE(
            void,             /* Return type */
            python_component, /* Parent class */
            on_update /* Name of function in C++ (must match Python name) */
        );
    }
    void on_deinit() override
    {
        PYBIND11_OVERRIDE(
            void,             /* Return type */
            python_component, /* Parent class */
            on_deinit /* Name of function in C++ (must match Python name) */
        );
    }
};

PYBIND11_EMBEDDED_MODULE(sample, m)
{
    pybind11::class_<glm::dvec3>(m, "vec3");
    pybind11::class_<game_object, std::shared_ptr<game_object>>(m,
                                                                "game_object");
    pybind11::class_<component, std::shared_ptr<component>>(m, "component")
        .def("transform", &component::get_transform);
    pybind11::class_<python_component,
                     python_component_trampoline,
                     std::shared_ptr<python_component>>(m, "python_component")
        .def(pybind11::init<game_object&>())
        .def("on_init", &python_component::on_init)
        .def("on_update", &python_component::on_update)
        .def("on_deinit", &python_component::on_deinit)
        .def("transform", &python_component::get_transform);
    pybind11::class_<components::transform,
                     std::shared_ptr<components::transform>>(m, "transform")
        .def("rotate", &components::transform::rotate)
        .def("get_right", &components::transform::get_right);
}

int main()
{
    pybind11::scoped_interpreter sc;

    project_manager::initialize();
    auto obj = game_object::create();
    obj->set_name("hello");

    std::shared_ptr<component> cmp;
    try
    {
        pybind11::module m = pybind11::module::import("module");

        for (auto it : m.attr("module_exports"))
        {
            auto cls = it.cast<pybind11::class_<python_component>>();

            auto pyobj = cls(*obj);
            cmp = pyobj.cast<std::shared_ptr<python_component>>();
            pyobj.cast<std::shared_ptr<python_component>>()->_instance = pyobj;
            // component_registry::register_component(
            //     cls.attr("name").cast<std::string>(),
            //     [ cls ](game_object& obj) -> std::shared_ptr<component>
            // {
            //     try
            //     {
            //         auto pyobj = cls(obj);
            //         return pyobj.cast<std::shared_ptr<python_component>>();
            //     }
            //     catch (std::exception& e)
            //     {
            //         get_logger("pybind_sample")
            //             ->error("Failed to create component: {}", e.what());
            //         return nullptr;
            //     }
            // });
        }
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    cmp->update();

    return 0;
}
