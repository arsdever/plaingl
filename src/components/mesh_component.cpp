#include "components/mesh_component.hpp"

mesh_component::mesh_component(game_object* parent)
    : component(parent, class_type_id)
{
}

void mesh_component::set_mesh(mesh* m) { _mesh = m; }

mesh* mesh_component::get_mesh() { return _mesh; }
