/* clang-format off */
#include "glad/gl.h"
#include "GLFW/glfw3.h"
/* clang-format on */

#include <array>

#include "mesh.hpp"

mesh::mesh() { }

mesh::mesh(mesh&& m)
{
    _vao = m._vao;
    _vbo = m._vbo;
    _ebo = m._ebo;
    _vao_map = std::move(m._vao_map);
    m._vao = 0;
    m._vbo = 0;
    m._ebo = 0;
    m._vao_map.clear();
}

mesh& mesh::operator=(mesh&& m)
{
    _vao = m._vao;
    _vbo = m._vbo;
    _ebo = m._ebo;
    _vao_map = std::move(m._vao_map);
    m._vao = 0;
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
    // TODO: generate vao per context
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

    std::array<std::array<float, 3>, 3> vertices {
        { { -.5, -.5, 0 }, { 0, .75, 0 }, { .5, -.5, 0 } },
    };

    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * vertices[ 0 ].size() *
                     sizeof(vertices[ 0 ][ 0 ]),
                 vertices.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::array<int, 3> indices { 0, 1, 2 };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(indices),
                 indices.data(),
                 GL_STATIC_DRAW);
    _vao_map[ glfwGetCurrentContext() ] = _vao;

    glBindVertexArray(0);
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
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
