#pragma once

#include "graphics/renderer/renderer.hpp"
#include "graphics/vaomap.hpp"

namespace graphics
{
class material;
}
class mesh;

class renderer_3d : public renderer
{
public:
    void draw_mesh(mesh* m, std::shared_ptr<graphics::material> mat);

private:
    vao_map _vao;
};
