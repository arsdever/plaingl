#pragma once

#include "component.hpp"

class collider_component : public component
{
public:
    collider_component(game_object* parent,
                       std::string_view type_id = class_type_id)
        : component(parent, type_id)
    {
    }

    static constexpr std::string_view class_type_id = "collider_component";
};
