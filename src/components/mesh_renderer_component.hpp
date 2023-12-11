#pragma once

#include "component.hpp"

class material;

class mesh_renderer_component : public component
{
public:
    mesh_renderer_component(game_object* parent);

    void set_material(material* mat);
    material* get_material();

    void update() override;

    static constexpr std::string_view class_type_id = "mesh_renderer_component";

private:
    material* _material = nullptr;
};
