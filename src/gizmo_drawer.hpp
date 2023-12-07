#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "shader.hpp"

class gizmo_drawer
{
public:
    void init();

    void draw_line(glm::vec3 p1, glm::vec3 p2, glm::vec4 color);
    void draw_line_2d(glm::vec2 p1, glm::vec2 p2, glm::vec4 color);

    shader_program& get_shader();

    static gizmo_drawer* instance();

private:
    shader_program _gizmo_shader;
    unsigned _color_location;
    static gizmo_drawer* _instance;
};
