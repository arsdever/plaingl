#include "component.hpp"

component::~component() = default;

void component::set_game_object(game_object* parent) { _game_object = parent; }

game_object* component::get_game_object() const { return _game_object; }
