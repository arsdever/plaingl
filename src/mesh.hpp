#pragma once

#include <unordered_map>
#include <vector>

#include <glm/vec3.hpp>

#include "base.hpp"
#include "vertex.hpp"

struct GLFWwindow;
class mesh_asset;

class mesh
{
public:
    mesh();
    mesh(mesh&& m);
    mesh(const mesh& m) = delete;
    mesh& operator=(mesh&& m);
    mesh& operator=(const mesh& m) = delete;
    ~mesh();

    void init();

    void set_mesh_asset(sp<mesh_asset> asset);

    void set_vertices(std::vector<vertex> positions);
    void set_indices(std::vector<int> indices);

    // TODO: not the best approach
    // SUGGESTION: move the logic into the renderer class. The last will also
    // manage the vao creation per context
    void render();

private:
    sp<mesh_asset> _mesh_asset = nullptr;

    std::vector<vertex> _vertices;
    std::vector<int> _indices;

    std::unordered_map<GLFWwindow*, unsigned int> _vao_map;
    unsigned int _vbo = 0;
    unsigned int _ebo = 0;
    bool _needs_update = true;
};
