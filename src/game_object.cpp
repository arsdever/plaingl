#include "game_object.hpp"

#include "mesh_renderer.hpp"

game_object::game_object()
    : _mesh_renderer(new mesh_renderer)
{
    _mesh_renderer->set_game_object(this);
}

void game_object::set_selected(bool selected) { _selected = selected; }

bool game_object::is_selected() const { return _selected; }

void game_object::update() { _mesh_renderer->render(); }

transform& game_object::get_transform() { return _transformation; }

const transform& game_object::get_transform() const { return _transformation; }

template <>
mesh_renderer* game_object::get_component<mesh_renderer>() const
{
    return static_cast<mesh_renderer*>(_mesh_renderer);
}
