#include "components/walking_component.hpp"

#include "game_clock.hpp"
#include "experimental/input_system.hpp"

walking_component::walking_component(game_object* parent)
    : component(parent, class_type_id)
{
}

void walking_component::update()
{
    float speed = 2;
    auto old_position = get_game_object()->get_transform().get_position();
    auto forward = get_game_object()->get_transform().get_rotation() *
                   glm::vec3 { 0, 0, 1 };
    auto right = get_game_object()->get_transform().get_rotation() *
                 glm::vec3 { 1, 0, 0 };
    auto up = get_game_object()->get_transform().get_rotation() *
              glm::vec3 { 0, 1, 0 };
    glm::vec3 transformation {};

    if (experimental::input_system::is_key_down('W'))
    {
        transformation +=
            speed * static_cast<float>(game_clock::delta().count()) * forward;
    }
    if (experimental::input_system::is_key_down('S'))
    {
        transformation -=
            speed * static_cast<float>(game_clock::delta().count()) * forward;
    }
    if (experimental::input_system::is_key_down('A'))
    {
        transformation +=
            speed * static_cast<float>(game_clock::delta().count()) * right;
    }
    if (experimental::input_system::is_key_down('D'))
    {
        transformation -=
            speed * static_cast<float>(game_clock::delta().count()) * right;
    }
    if (experimental::input_system::is_key_down(' '))
    {
        transformation +=
            speed * static_cast<float>(game_clock::delta().count()) * up;
    }
    if (experimental::input_system::is_key_down(341))
    {
        transformation -=
            speed * static_cast<float>(game_clock::delta().count()) * up;
    }

    get_game_object()->get_transform().set_position(old_position +
                                                    transformation);
}
