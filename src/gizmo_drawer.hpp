#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "shader.hpp"

class gizmo_drawer
{
public:
    void init();

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
    void draw_vertices(std::vector<glm::vec3> vertices);
    void draw_vertices(std::vector<glm::vec3> vertices,
                       std::vector<int> indices);

private:
    shader_program _gizmo_shader;
    static gizmo_drawer* _instance;
};
