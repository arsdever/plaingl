#include "components/mesh_renderer_component.hpp"

#include "components/mesh_component.hpp"
#include "logging.hpp"

namespace
{
static inline logger log()
{
    return get_logger(mesh_renderer_component::class_type_id);
}
} // namespace

mesh_renderer_component::mesh_renderer_component(game_object* parent)
    : component(parent, class_type_id)
{
    if (get_component<mesh_component>() == nullptr)
    {
        log()->error("requires mesh_component");
    }
}

void mesh_renderer_component::set_material(material* mat) { _material = mat; }

material* mesh_renderer_component::get_material() { return _material; }

void mesh_renderer_component::update()
{
    if (_material)
    {
        _material->set_property_value(
            "model_matrix", get_game_object()->get_transform().get_matrix());
        _material->activate();

        if (get_component<mesh_component>())
        {
            get_component<mesh_component>()->get_mesh()->render();
        }
        _material->deactivate();
    }
}
