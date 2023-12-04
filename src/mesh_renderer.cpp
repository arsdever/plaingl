#include "mesh_renderer.hpp"

#include "camera.hpp"
#include "game_object.hpp"
#include "material.hpp"
#include "shader.hpp"
#include "transform.hpp"

mesh_renderer::~mesh_renderer() = default;

void mesh_renderer::set_mesh(mesh* m) { _mesh = m; }

mesh* mesh_renderer::get_mesh() { return _mesh; }

void mesh_renderer::set_material(material* mat) { _material = mat; }

material* mesh_renderer::get_material() { return _material; }

void mesh_renderer::render()
{
    glm::mat4 mvp_matrix = camera::active_camera()->vp_matrix() *
                           get_game_object()->get_transform().get_matrix();
    _material->set_property("mvp_matrix", mvp_matrix);
    _material->activate();
    _mesh->render();
    shader_program::unuse();
}
