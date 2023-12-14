#include "components/mesh_renderer_component.hpp"

#include "components/mesh_component.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "mesh.hpp"

namespace
{
static inline logger log()
{
    return get_logger(mesh_renderer_component::class_type_id);
}
} // namespace

mesh_renderer_component::mesh_renderer_component(game_object* parent)
    : renderer_component(parent, class_type_id)
{
    if (get_component<mesh_component>() == nullptr)
    {
        log()->error("requires mesh_component");
    }
}

void mesh_renderer_component::render()
{
    if (_material)
    {
        _material->activate();

        if (get_component<mesh_component>())
        {
            get_component<mesh_component>()->get_mesh()->render();
        }
        _material->deactivate();
    }
}
