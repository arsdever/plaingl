#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "shader.hpp"
#include "vaomap.hpp"

struct GLFWwindow;

class gizmo_drawer
{
public:
    void init();

    void draw_grid(glm::vec3 position,
                   glm::quat rotation,
                   glm::vec2 scale,
                   size_t count,
                   float distance,
                   glm::vec4 color = { 1.0f, 1.0f, 1.0f, 0.3f },
                   bool force = false);
    void draw_plane(glm::vec3 position,
                    glm::quat rotation,
                    glm::vec2 scale,
                    glm::vec4 color);
    void draw_box(glm::vec3 position,
                  glm::quat rotation,
                  glm::vec3 scale,
                  glm::vec4 color);
    void draw_sphere(glm::vec3 center, float radius, glm::vec4 color);
    void draw_ray(glm::vec3 pos, glm::vec3 dir, float length, glm::vec4 color);
    void draw_line(glm::vec3 p1, glm::vec3 p2, glm::vec4 color);
    void draw_line_2d(glm::vec2 p1, glm::vec2 p2, glm::vec4 color);

    shader_program& get_shader();

    static gizmo_drawer* instance();

private:
    void draw_vertices(const std::vector<glm::vec3>& vertices);
    void draw_vertices(const std::vector<glm::vec3>& vertices,
                       const std::vector<int>& indices,
                       unsigned& vbo,
                       unsigned& ebo);

private:
    shader_program _gizmo_shader;
    static gizmo_drawer* _instance;
    std::vector<glm::vec3> _grid_vertices_cache;
    std::vector<int> _grid_indices_cache;
    size_t _grid_cache_checksum = 0;
    vao_map _vao;
    unsigned _vbo = 0;
    unsigned _ebo = 0;
    unsigned _grid_vbo = 0;
    unsigned _grid_ebo = 0;
};
