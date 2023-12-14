#pragma once

#include "component.hpp"

class camera;

class camera_component : public component
{
public:
    camera_component(game_object* parent);

    void set_camera(camera* m);
    camera* get_camera();

    void draw_gizmos() override;

    static constexpr std::string_view class_type_id = "camera_component";

private:
    camera* _camera = nullptr;
};
