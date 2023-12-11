#pragma once

#include "component.hpp"

class jumpy_component : public component
{
public:
    jumpy_component(game_object* parent);

    void update();

    static constexpr std::string_view class_type_id = "jumpy_component";

private:
    glm::vec3 _velocity;
};
