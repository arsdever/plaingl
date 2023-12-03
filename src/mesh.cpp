/* clang-format off */
#include "glad/gl.h"
#include "GLFW/glfw3.h"
/* clang-format on */

#include <array>

#include "mesh.hpp"

mesh::mesh() { }

mesh::mesh(mesh&& m)
{
    _vbo = m._vbo;
    _ebo = m._ebo;
    _vao_map = std::move(m._vao_map);
    m._vbo = 0;
    m._ebo = 0;
    m._vao_map.clear();
}

mesh& mesh::operator=(mesh&& m)
{
    _vbo = m._vbo;
    _ebo = m._ebo;
    _vao_map = std::move(m._vao_map);
    m._vbo = 0;
    m._ebo = 0;
    m._vao_map.clear();
    return *this;
}

mesh::~mesh()
{
    for (auto [ window, vao ] : _vao_map)
    {
        glfwMakeContextCurrent(window);
        glDeleteVertexArrays(1, &vao);
    }

    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
}

void mesh::init()
{
    if (_vbo == 0)
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    }

    if (_ebo == 0)
    {
        glGenBuffers(1, &_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    }

    glBufferData(GL_ARRAY_BUFFER,
                 _vertex_positions.size() * 3 * sizeof(glm::vec3::type),
                 _vertex_positions.data(),
                 GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 _vertex_indices.size() * sizeof(int),
                 _vertex_indices.data(),
                 GL_STATIC_DRAW);
}

void mesh::set_vertices(std::vector<glm::vec3> positions)
{
    _vertex_positions = std::move(positions);
}

void mesh::set_indices(std::vector<int> indices)
{
    _vertex_indices = std::move(indices);
}

void mesh::render()
{
    if (!_vao_map.contains(glfwGetCurrentContext()))
    {
        _vao_map[ glfwGetCurrentContext() ] = 0;
        glGenVertexArrays(1, &_vao_map[ glfwGetCurrentContext() ]);
        glBindVertexArray(_vao_map[ glfwGetCurrentContext() ]);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
    else
    {
        glBindVertexArray(_vao_map[ glfwGetCurrentContext() ]);
        glEnableVertexAttribArray(0);
    }
    glDrawElements(GL_TRIANGLES, _vertex_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
