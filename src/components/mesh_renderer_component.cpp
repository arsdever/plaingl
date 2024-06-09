#include "components/mesh_renderer_component.hpp"

#include "camera.hpp"
#include "components/mesh_component.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "renderer/renderer_3d.hpp"

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
        if (get_component<mesh_component>())
        {
            if (auto* mesh = get_component<mesh_component>()->get_mesh())
            {
                renderer_3d().draw_mesh(mesh, _material);
            }
        }
    }
}
