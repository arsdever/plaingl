#pragma once

#include "component.hpp"

class light;

class light_component : public component
{
public:
    light_component(game_object* parent);

    void set_light(light* l);
    light* get_light();

    void update() override;
    void draw_gizmos() override;

    static constexpr std::string_view class_type_id = "light_component";

private:
    light* _light = nullptr;
};
