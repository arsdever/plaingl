#pragma once

#include "material.hpp"
#include "mesh.hpp"

class game_object
{
public:
    game_object();

    void set_mesh(mesh* m);
    void set_material(material mat);

    mesh* get_mesh();
    material& get_material();

    void set_selected(bool selected = true);
    bool is_selected() const;

    void update();

private:
    mesh* _mesh;
    material _material;
    bool _selected = false;
};
