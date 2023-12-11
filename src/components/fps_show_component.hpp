#pragma once

#include "component.hpp"

class fps_show_component : public component
{
public:
    fps_show_component(game_object* parent);

    void update();

    static constexpr std::string_view class_type_id = "fps_show_component";
};
