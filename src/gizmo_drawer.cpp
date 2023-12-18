#include <array>

#include <glad/gl.h>
#include <glm/ext.hpp>

#include "gizmo_drawer.hpp"

#include "glm/gtx/quaternion.hpp"

void gizmo_drawer::init()
{
    _gizmo_shader.init();
    _gizmo_shader.add_shader("gizmo.vert");
    _gizmo_shader.add_shader("gizmo.frag");
    _gizmo_shader.link();
    _gizmo_shader.use();
    shader_program::unuse();
}

void gizmo_drawer::draw_box(glm::vec3 position,
                            glm::quat rotation,
                            glm::vec3 scale,
                            glm::vec4 color)
{
    std::vector<glm::vec3> vertices {
        { -.5, -.5, -.5 }, { -.5, -.5, .5 }, { -.5, .5, -.5 }, { -.5, .5, .5 },
        { .5, -.5, -.5 },  { .5, -.5, .5 },  { .5, .5, -.5 },  { .5, .5, .5 },
    };
    std::vector<int> indices { 0, 1, 1, 5, 5, 4, 4, 0, 2, 3, 3, 7,
                               7, 6, 6, 2, 2, 0, 6, 4, 7, 5, 3, 1 };

    glm::mat4 transform = glm::identity<glm::mat4>();
    transform = glm::translate(transform, position);
    transform = transform * glm::toMat4(rotation);
    transform = glm::scale(transform, scale);

    for (auto& v : vertices)
    {
        v = transform * glm::vec4 { v, 1 };
    }

    _gizmo_shader.set_uniform(
        "color", std::make_tuple(color.r, color.g, color.b, color.a));
    _gizmo_shader.use();
    draw_vertices(std::move(vertices), std::move(indices));
    shader_program::unuse();
}

void gizmo_drawer::draw_sphere(glm::vec3 center, float radius, glm::vec4 color)
{
    std::vector<glm::vec3> vertices;
    std::vector<int> indices;
    for (int i = 0; i < 12; ++i)
    {
        float angle = static_cast<float>(i) / 12.0f * glm::pi<float>() * 2.0f;
        vertices.emplace_back(
            0, std::sin(angle) * radius, std::cos(angle) * radius);
        vertices.emplace_back(
            std::sin(angle) * radius, 0, std::cos(angle) * radius);
        vertices.emplace_back(
            std::sin(angle) * radius, std::cos(angle) * radius, 0);
    }

    for (int j = 0; j < 3; ++j)
    {
        for (int i = 0; i < 12; ++i)
        {
            indices.push_back(j + i * 3);
            indices.push_back(j + ((i + 1) % 12) * 3);
        }
    }

    _gizmo_shader.set_uniform(
        "color", std::make_tuple(color.r, color.g, color.b, color.a));
    _gizmo_shader.use();
    draw_vertices(std::move(vertices), std::move(indices));
    shader_program::unuse();
}

void gizmo_drawer::draw_ray(glm::vec3 pos,
                            glm::vec3 dir,
                            float length,
                            glm::vec4 color)
{
    glm::vec3 endpoint = pos + dir * length;
    glm::quat look_rotation = glm::quatLookAt(dir, { 0, 1, 0 });
    glm::vec3 right = look_rotation *
                      glm::quat(glm::radians(glm::vec3 { 0, 210, 0 })) *
                      glm::vec3 { 0, 0, 1 };
    glm::vec3 left = look_rotation *
                     glm::quat(glm::radians(glm::vec3 { 0, 150, 0 })) *
                     glm::vec3 { 0, 0, 1 };
    glm::vec3 up = look_rotation *
                   glm::quat(glm::radians(glm::vec3 { 210, 0, 0 })) *
                   glm::vec3 { 0, 0, 1 };
    glm::vec3 down = look_rotation *
                     glm::quat(glm::radians(glm::vec3 { 150, 0, 0 })) *
                     glm::vec3 { 0, 0, 1 };

    draw_line(pos, endpoint, color);
    draw_line(endpoint, endpoint - right * length * 0.1f, color);
    draw_line(endpoint, endpoint - left * length * 0.1f, color);
    draw_line(endpoint, endpoint - up * length * 0.1f, color);
    draw_line(endpoint, endpoint - down * length * 0.1f, color);
}
void gizmo_drawer::draw_line(glm::vec3 p1, glm::vec3 p2, glm::vec4 color)
{
    _gizmo_shader.set_uniform(
        "color", std::make_tuple(color.r, color.g, color.b, color.a));
    _gizmo_shader.use();
    draw_vertices({ { p1 }, { p2 } });
    shader_program::unuse();
}

void gizmo_drawer::draw_line_2d(glm::vec2 p1, glm::vec2 p2, glm::vec4 color)
{
    _gizmo_shader.set_uniform(
        "color", std::make_tuple(color.r, color.g, color.b, color.a));
    _gizmo_shader.use();
    draw_vertices({ { p1, 0 }, { p2, 0 } });
    shader_program::unuse();
}

void gizmo_drawer::draw_vertices(std::vector<glm::vec3> vertices)
{
    std::vector<int> indices(vertices.size(), 0);
    for (int i = 0; i < vertices.size(); ++i)
    {
        indices[ i ] = i;
    }
    draw_vertices(std::move(vertices), std::move(indices));
}

void gizmo_drawer::draw_vertices(std::vector<glm::vec3> vertices,
                                 std::vector<int> indices)
{
    unsigned vao;
    unsigned vbo;
    unsigned ebo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ARRAY_BUFFER,
                 // number of vertices
                 vertices.size() *
                     // glm::vec component's type size
                     sizeof(decltype(vertices)::value_type::value_type) *
                     // glm::vec component count
                     sizeof(decltype(vertices)::value_type::length()),
                 vertices.data(),
                 GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(decltype(vertices)::value_type),
                 indices.data(),
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
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
