/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <glm/ext.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/quaternion.hpp>

#include "gizmo_drawer.hpp"

#include "vertex.hpp"

void gizmo_drawer::init()
{
    _gizmo_shader.init();
    _gizmo_shader.add_shader("gizmo.vert");
    _gizmo_shader.add_shader("gizmo.frag");
    _gizmo_shader.link();
    _gizmo_shader.use();
    shader_program::unuse();
}

void gizmo_drawer::draw_grid(glm::vec3 position,
                             glm::quat rotation,
                             glm::vec2 scale,
                             size_t count,
                             float distance,
                             glm::vec4 color,
                             bool force)
{
    glm::mat4 transform = glm::identity<glm::mat4>();
    transform = glm::translate(transform, position);
    transform = transform * glm::toMat4(rotation);
    transform = glm::scale(transform, { scale, 1 });
    size_t hash = std::hash<glm::mat4> {}(transform);

    if (force || _grid_cache_checksum != hash)
    {
        std::vector<glm::vec3> vertices;
        float max_far = count * distance;
        for (size_t i = 0; i <= count; ++i)
        {
            vertices.push_back(
                { -max_far,
                  (static_cast<float>(i) - static_cast<float>(count) / 2.0f) *
                      distance * 2.0f,
                  0 });
            vertices.push_back(
                { max_far,
                  (static_cast<float>(i) - static_cast<float>(count) / 2.0f) *
                      distance * 2.0f,
                  0 });
            vertices.push_back(
                { (static_cast<float>(i) - static_cast<float>(count) / 2.0f) *
                      distance * 2.0f,
                  -max_far,
                  0 });
            vertices.push_back(
                { (static_cast<float>(i) - static_cast<float>(count) / 2.0f) *
                      distance * 2.0f,
                  max_far,
                  0 });
        }

        for (auto& v : vertices)
        {
            v = transform * glm::vec4 { v, 1 };
        }
        _grid_vertices_cache = std::move(vertices);

        std::vector<int> indices(_grid_vertices_cache.size(), 0);
        for (int i = 0; i < _grid_vertices_cache.size(); ++i)
        {
            indices[ i ] = i;
        }
        _grid_indices_cache = std::move(indices);
    }

    _gizmo_shader.set_uniform(
        "color", std::make_tuple(color.r, color.g, color.b, color.a));
    _gizmo_shader.use();
    draw_vertices(
        _grid_vertices_cache, _grid_indices_cache, _grid_vbo, _grid_ebo);
    shader_program::unuse();
}

void gizmo_drawer::draw_plane(glm::vec3 position,
                              glm::quat rotation,
                              glm::vec2 scale,
                              glm::vec4 color)
{
    std::vector<glm::vec3> vertices {
        { -.5, -.5, 0 },
        { -.5, .5, 0 },
        { .5, -.5, 0 },
        { .5, .5, 0 },
    };
    std::vector<int> indices { 0, 1, 1, 3, 3, 2, 2, 0 };

    glm::mat4 transform = glm::identity<glm::mat4>();
    transform = glm::translate(transform, position);
    transform = transform * glm::toMat4(rotation);
    transform = glm::scale(transform, { scale, 1 });

    for (auto& v : vertices)
    {
        v = transform * glm::vec4 { v, 1 };
    }

    _gizmo_shader.set_uniform(
        "color", std::make_tuple(color.r, color.g, color.b, color.a));
    _gizmo_shader.use();
    draw_vertices(std::move(vertices), std::move(indices), _vbo, _ebo);
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
    draw_vertices(std::move(vertices), std::move(indices), _vbo, _ebo);
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
    draw_vertices(std::move(vertices), std::move(indices), _vbo, _ebo);
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

void gizmo_drawer::draw_vertices(const std::vector<glm::vec3>& vertices)
{
    std::vector<int> indices(vertices.size(), 0);
    for (int i = 0; i < vertices.size(); ++i)
    {
        indices[ i ] = i;
    }
    draw_vertices(vertices, indices, _vbo, _ebo);
}

void gizmo_drawer::draw_vertices(const std::vector<glm::vec3>& vertices,
                                 const std::vector<int>& indices,
                                 unsigned& vbo,
                                 unsigned& ebo)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ARRAY_BUFFER,
        // number of vertices
        vertices.size() *
            // glm::vec component's type size
            sizeof(std::remove_cvref_t<
                   decltype(vertices)>::value_type::value_type) *
            // glm::vec component count
            sizeof(
                std::remove_cvref_t<decltype(vertices)>::value_type::length()),
        vertices.data(),
        GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() *
                     sizeof(std::remove_cvref_t<decltype(indices)>::value_type),
                 indices.data(),
                 GL_STATIC_DRAW);

    simple_vertex3d::activate_attributes();
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
}

shader_program& gizmo_drawer::get_shader() { return _gizmo_shader; }

gizmo_drawer* gizmo_drawer::instance()
{
    if (!_instance)
    {
        _instance = new gizmo_drawer;
        _instance->init();
        glGenBuffers(1, &_instance->_vbo);
        glGenBuffers(1, &_instance->_ebo);
    }

    if (!_instance->_vao.activate())
    {
        glBindBuffer(GL_ARRAY_BUFFER, _instance->_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _instance->_ebo);
        simple_vertex3d::initialize_attributes();
    }

    return _instance;
}

gizmo_drawer* gizmo_drawer::_instance = nullptr;
