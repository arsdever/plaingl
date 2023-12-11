#pragma once

#include "component.hpp"

class mesh;

class mesh_component : public component
{
public:
    mesh_component(game_object* parent);

    void set_mesh(mesh* m);
    mesh* get_mesh();

    static constexpr std::string_view class_type_id = "mesh_component";

private:
    mesh* _mesh = nullptr;
};
