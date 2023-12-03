#pragma once

#include <unordered_map>
#include <vector>

#include <glm/vec3.hpp>

struct GLFWwindow;

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

    void set_vertices(std::vector<glm::vec3> positions);
    void set_indices(std::vector<int> indices);

    // TODO: not the best approach
    // SUGGESTION: move the logic into the renderer class. The last will also
    // manage the vao creation per context
    void render();

private:
    std::vector<glm::vec3> _vertex_positions;
    std::vector<int> _vertex_indices;

    std::unordered_map<GLFWwindow*, unsigned int> _vao_map;
    unsigned int _vbo = 0;
    unsigned int _ebo = 0;
};
