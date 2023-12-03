#pragma once

#include "material.hpp"
#include "mesh.hpp"
#include "transform.hpp"

class game_object
{
public:
    game_object();

    void set_mesh(mesh* m);
    void set_material(material* mat);

    mesh* get_mesh();
    material* get_material();

    void set_selected(bool selected = true);
    bool is_selected() const;

    void update();

    transform& get_transform();
    const transform& get_transform() const;

private:
    transform _transformation;
    mesh* _mesh;
    material* _material;
    bool _selected = false;
};
