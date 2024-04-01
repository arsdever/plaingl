#include "game_object.hpp"

#include "component.hpp"
#include "components/transform_component.hpp"

game_object::game_object() { create_component<transform_component>(); }

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
    if (!is_active())
    {
        return;
    }

    for (auto& c : _components)
    {
        c->update();
    }
}

void game_object::draw_gizmos()
{
    if (!is_active())
    {
        return;
    }

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

void game_object::set_active(bool active_flag) { _is_active = active_flag; }

bool game_object::is_active() const { return _is_active; }

void game_object::set_name(std::string_view name)
{
    _name = std::string { name };
}

std::string_view game_object::get_name() const { return _name; }

transform& game_object::get_transform() { return _transformation; }

const transform& game_object::get_transform() const { return _transformation; }

void game_object::add_child(game_object* child)
{
    child->set_parent(this);
    _children.push_back(child);
}

std::vector<game_object*>& game_object::get_children() { return _children; }

void game_object::set_parent(game_object* parent) { _parent = parent; }

game_object* game_object::get_parent() { return _parent; }
