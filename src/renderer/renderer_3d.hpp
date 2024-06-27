#pragma once

#include "renderer/renderer.hpp"
#include "graphics/vaomap.hpp"

class material;
class mesh;

class renderer_3d : public renderer
{
public:
    void draw_mesh(mesh* m, material* mat);

private:
    vao_map _vao;
};
