#pragma once

#include "renderer.hpp"

class material;
class mesh;

class mesh_renderer : public renderer
{
public:
    ~mesh_renderer();

    void set_mesh(mesh* m);
    mesh* get_mesh();

    void set_material(material* mat);
    material* get_material();

    void render() override;

private:
    material* _material;
    mesh* _mesh;
};
