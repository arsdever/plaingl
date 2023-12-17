#pragma once

#include "component.hpp"

class walking_component : public component
{
public:
    walking_component(game_object* parent);

    void update() override;

    static constexpr std::string_view class_type_id = "walking_component";
};
