/* clang-format off */
#include "glad/gl.h"
#include "GLFW/glfw3.h"
/* clang-format on */

#include "mesh.hpp"

mesh::mesh() { }

mesh::mesh(mesh&& m)
{
    _vbo = m._vbo;
    _ebo = m._ebo;
    _vao = std::move(m._vao);
    m._vbo = 0;
    m._ebo = 0;
}

mesh& mesh::operator=(mesh&& m)
{
    _vbo = m._vbo;
    _ebo = m._ebo;
    _vao = std::move(m._vao);
    m._vbo = 0;
    m._ebo = 0;
    return *this;
}

mesh::~mesh()
{
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
                 _vertices.size() * vertex3d::size,
                 _vertices.data(),
                 GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 _indices.size() * sizeof(int),
                 _indices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void mesh::set_vertices(std::vector<vertex3d> positions)
{
    _vertices = std::move(positions);
}

void mesh::set_indices(std::vector<int> indices)
{
    _indices = std::move(indices);
}

void mesh::render()
{
    if (_vao.activate())
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        vertex3d::initialize_attributes();
    }

    vertex3d::activate_attributes();
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
