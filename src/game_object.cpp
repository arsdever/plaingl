#include "game_object.hpp"

#include "component.hpp"

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

void game_object::draw_gizmos()
{
    for (auto& c : _components)
    {
        c->draw_gizmos();
    }
}

void game_object::deinit()
{
    for (auto& c : _components)
    {
        c->deinit();
    }
}

transform& game_object::get_transform() { return _transformation; }

const transform& game_object::get_transform() const { return _transformation; }
