#pragma once

#include "material.hpp"
#include "mesh.hpp"

class game_object
{
public:
    game_object();

    void set_mesh(mesh m);
    void set_material(material mat);

    mesh& get_mesh();
    material& get_material();

    void update();

private:
    mesh _mesh;
    material _material;
};
