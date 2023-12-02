#include "game_object.hpp"

#include "shader.hpp"

game_object::game_object() = default;

void game_object::set_mesh(mesh m) { _mesh = std::move(m); }

void game_object::set_material(material mat) { _material = std::move(mat); }

void game_object::update()
{
    _material.activate();
    _mesh.render();
    shader_program::unuse();
}
