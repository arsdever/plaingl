#pragma once

#include <unordered_map>
#include <vector>

#include <glm/vec3.hpp>

#include "vaomap.hpp"
#include "vertex.hpp"

struct GLFWwindow;

class mesh
{
public:
    struct sub_mesh_information
    {
        int _start_index;
        int _index_count;
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

    void add_sub_mesh(sub_mesh_information submesh_info);
    void set_sub_mesh(size_t index, sub_mesh_information submesh_info);
    void set_sub_meshes(std::vector<sub_mesh_information> submeshes);

    // TODO: not the best approach
    // SUGGESTION: move the logic into the renderer class. The last will also
    // manage the vao creation per context
    void render();

private:
    std::vector<vertex3d> _vertices;
    std::vector<int> _indices;
    std::vector<sub_mesh_information> _sub_meshes;

    vao_map _vao;
    unsigned int _vbo = 0;
    unsigned int _ebo = 0;
};
