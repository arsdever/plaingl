#pragma once

#include "renderer/renderer.hpp"

class renderer_2d : public renderer
{
public:
    void draw_rect(glm::vec2 top_left,
                   glm::vec2 bottom_right,
                   float border_thickness,
                   glm::vec4 border_color,
                   glm::vec4 fill_color);
};
