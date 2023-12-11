#include "game_object.hpp"

#include "component.hpp"
#include "gizmo_drawer.hpp"
#include "shader.hpp"

game_object::game_object() = default;

void game_object::set_selected(bool selected) { _selected = selected; }

bool game_object::is_selected() const { return _selected; }

void game_object::init()
{
    for (auto& c : _components)
    {
        c->init();
    }
}

void game_object::update()
{
    for (auto& c : _components)
    {
        c->update();
    }
}

void game_object::deinit()
{
    for (auto& c : _components)
    {
        c->deinit();
    }
}

void game_object::draw_gizmos()
{
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
