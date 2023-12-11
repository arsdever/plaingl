#include <array>

#include <glad/gl.h>

#include "gizmo_drawer.hpp"

void gizmo_drawer::init()
{
    _gizmo_shader.init();
    _gizmo_shader.add_shader("gizmo.vert");
    _gizmo_shader.add_shader("gizmo.frag");
    _gizmo_shader.link();
    _gizmo_shader.use();
    _color_location = glGetUniformLocation(_gizmo_shader.id(), "color");
    shader_program::unuse();
}

void gizmo_drawer::draw_line(glm::vec3 p1, glm::vec3 p2, glm::vec4 color)
{
    _gizmo_shader.use();
    unsigned vao;
    unsigned vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    std::array<glm::vec3, 2> verts { { p1, p2 } };
    glBufferData(GL_ARRAY_BUFFER,
                 // number of vertices
                 verts.size() *
                     // glm::vec component's type size
                     sizeof(decltype(verts)::value_type::value_type) *
                     // glm::vec component count
                     sizeof(decltype(verts)::value_type::length()),
                 verts.data(),
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glUniform4fv(_color_location, 1, glm::value_ptr(color));
    glDrawArrays(GL_LINES, 0, 2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    shader_program::unuse();
}

void gizmo_drawer::draw_line_2d(glm::vec2 p1, glm::vec2 p2, glm::vec4 color)
{
    _gizmo_shader.use();
    unsigned vao;
    unsigned vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    std::array<glm::vec3, 2> verts { { { p1, 0 }, { p2, 0 } } };
    glBufferData(GL_ARRAY_BUFFER,
                 // number of vertices
                 verts.size() *
                     // glm::vec component's type size
                     sizeof(decltype(verts)::value_type::value_type) *
                     // glm::vec component count
                     sizeof(decltype(verts)::value_type::length()),
                 verts.data(),
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glUniform4fv(_color_location, 1, glm::value_ptr(color));
    glDrawArrays(GL_LINES, 0, 2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    shader_program::unuse();
}

shader_program& gizmo_drawer::get_shader() { return _gizmo_shader; }

gizmo_drawer* gizmo_drawer::instance()
{
    if (!_instance)
    {
        _instance = new gizmo_drawer;
        _instance->init();
    }

    return _instance;
}

gizmo_drawer* gizmo_drawer::_instance = nullptr;
