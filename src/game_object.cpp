#include "game_object.hpp"

#include "gizmo_drawer.hpp"
#include "shader.hpp"

game_object::game_object() = default;

void game_object::set_mesh(mesh* m) { _mesh = m; }

void game_object::set_material(material* mat) { _material = mat; }

mesh* game_object::get_mesh() { return _mesh; }

material* game_object::get_material() { return _material; }

void game_object::set_selected(bool selected) { _selected = selected; }

bool game_object::is_selected() const { return _selected; }

void game_object::update()
{
    if (_material)
    {
        _material->set_property_value("model_matrix",
                                      _transformation.get_matrix());
        _material->set_property_value("is_selected",
                                      static_cast<int>(_selected));
        _material->activate();
    }
    if (_mesh)
    {
        _mesh->render();
    }
    shader_program::unuse();
}

void game_object::draw_gizmos()
{
    float size = 3;
    gizmo_drawer::instance()->get_shader().set_uniform(
        "model_matrix", std::make_tuple(_transformation.get_matrix()));
    gizmo_drawer::instance()->draw_line(
        { 0, 0, 0 }, { 0, 0, 1 }, { 0, 0, 1, 1 });
    gizmo_drawer::instance()->draw_line(
        { 0, 0, 0 }, { 0, 1, 0 }, { 0, 1, 0, 1 });
    gizmo_drawer::instance()->draw_line(
        { 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 0, 1 });
}

transform& game_object::get_transform() { return _transformation; }

const transform& game_object::get_transform() const { return _transformation; }
