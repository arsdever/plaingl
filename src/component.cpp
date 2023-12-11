#include "component.hpp"

#include "logging.hpp"

component::component(game_object* parent, std::string_view type_id)
    : _parent(parent)
    , _type_id(type_id)
{
}

component::~component() = default;

std::string_view component::type_id() const { return _type_id; }

void component::init()
{
    // by default do nothing
}

void component::update()
{
    // by default do nothing
}

void component::deinit()
{
    // by default do nothing
}

game_object* component::get_game_object() const { return _parent; }
