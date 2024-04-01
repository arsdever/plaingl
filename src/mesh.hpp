#pragma once

#include <unordered_map>
#include <vector>

#include <glm/vec3.hpp>

#include "vertex.hpp"
#include "vaomap.hpp"

struct GLFWwindow;

class mesh
{
public:
    struct submesh_info
    {
        size_t vertex_index_offset;
        unsigned short material_index;
    };

public:
    mesh();
    mesh(mesh&& m);
    mesh(const mesh& m) = delete;
    mesh& operator=(mesh&& m);
    mesh& operator=(const mesh& m) = delete;
    ~mesh();

    void init();

    void set_vertices(std::vector<vertex3d> positions);
    void set_indices(std::vector<int> indices);
    void set_submeshes(std::vector<submesh_info> submeshes);

    // TODO: not the best approach
    // SUGGESTION: move the logic into the renderer class. The last will also
    // manage the vao creation per context
    void render();

private:
    std::vector<vertex3d> _vertices;
    std::vector<int> _indices;
    std::vector<submesh_info> _submeshes;

    vao_map _vao;
    unsigned int _vbo = 0;
    unsigned int _ebo = 0;
};
