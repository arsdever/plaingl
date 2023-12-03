#include "game_object.hpp"

#include "camera.hpp"
#include "shader.hpp"

game_object::game_object() = default;

void game_object::set_mesh(mesh m) { _mesh = std::move(m); }

void game_object::set_material(material mat) { _material = std::move(mat); }

mesh& game_object::get_mesh() { return _mesh; }

material& game_object::get_material() { return _material; }

void game_object::update()
{
    glm::mat4 model = glm::identity<glm::mat4>();
    glm::mat4 mvp_matrix = model * camera::active_camera()->vp_matrix();

    _material.set_property("mvp_matrix", mvp_matrix);
    _material.activate();
    _mesh.render();
    shader_program::unuse();
}
