#include "components/mesh_renderer_component.hpp"

#include "camera.hpp"
#include "components/mesh_component.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "mesh.hpp"

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
        if (auto cam = camera::active_camera())
        {
            _material->set_property_value("u_vp_matrix", cam->vp_matrix());
            _material->set_property_value("u_camera_position",
                                          cam->get_transform().get_position());
        }

        _material->activate();

        if (get_component<mesh_component>())
        {
            if (auto* mesh = get_component<mesh_component>()->get_mesh())
            {
                mesh->render();
            }
        }
        _material->deactivate();
    }
}
