#include "components/jumpy_component.hpp"

#include "game_clock.hpp"
#include "transform.hpp"

jumpy_component::jumpy_component(game_object* parent)
    : component(parent, class_type_id)
{
}

void jumpy_component::update()
{
    glm::vec3 pos = get_game_object()->get_transform().get_position();
    pos.y = std::sin(game_clock::absolute().count() * 10);
    get_game_object()->get_transform().set_position(pos);
}
