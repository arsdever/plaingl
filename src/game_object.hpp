#pragma once

#include "mesh.hpp"
#include "material.hpp"

class game_object
{
public:
    game_object();

    void set_mesh(mesh m);
    void set_material(material mat);

    void update();
private:
    mesh _mesh;
    material _material;
};
